// Fill out your copyright notice in the Description page of Project Settings.


#include "UnicornCardGameManager.h"
#include "UnicornCardActor.h"
#include "UnicornSettings.h"

void UUnicornCardGameManager::AddToDrawPile(AUnicornCardActor* Card)
{
	DrawPile.Add(Card);
	OnDrawPileChanged.Broadcast(Card, false);
}

void UUnicornCardGameManager::RemoveFromDrawPile(AUnicornCardActor* Card)
{
	DrawPile.Remove(Card);
	OnDrawPileChanged.Broadcast(Card, true);
}

void UUnicornCardGameManager::AddToDiscardPile(AUnicornCardActor* Card)
{
	DiscardPile.Add(Card);
	OnDiscardPileChanged.Broadcast(Card, false);
}

void UUnicornCardGameManager::RemoveFromDiscardPile(AUnicornCardActor* Card)
{
	DiscardPile.Remove(Card);
	OnDiscardPileChanged.Broadcast(Card, true);
}

void UUnicornCardGameManager::AddToNursery(AUnicornCardActor* Card)
{
	Nursery.Add(Card);
	OnNurseryPileChanged.Broadcast(Card, false);
}

void UUnicornCardGameManager::RemoveFromNursery(AUnicornCardActor* Card)
{
	Nursery.Remove(Card);
	OnNurseryPileChanged.Broadcast(Card, true);
}

void UUnicornCardGameManager::AddToPlayerHand(int32 PlayerIndex, AUnicornCardActor* Card)
{	
	PlayerBoards.Find(PlayerIndex)->Hand.Add(Card);
	OnPlayerHandChanged.Broadcast(PlayerIndex, Card, false);
	Card->OnEnterHand();
}

void UUnicornCardGameManager::RemoveFromPlayerHand(int32 PlayerIndex, AUnicornCardActor* Card)
{
	PlayerBoards.Find(PlayerIndex)->Hand.Remove(Card);
	OnPlayerHandChanged.Broadcast(PlayerIndex, Card, true);
}

void UUnicornCardGameManager::AddToPlayerStable(int32 PlayerIndex, AUnicornCardActor* Card)
{
	if (Card->GetCardInfo().CardType >= ECardType::MagicalUnicorn)
	{
		PlayerBoards.Find(PlayerIndex)->StableUnicorns.Add(Card);
	}
	else
	{
		PlayerBoards.Find(PlayerIndex)->StableEffects.Add(Card);
	}
	CopyOfStables.Add(Card);
	OnPlayerStableChanged.Broadcast(PlayerIndex, Card, false);
	Card->OnEnterStable();
}

void UUnicornCardGameManager::RemoveFromPlayerStable(int32 PlayerIndex, AUnicornCardActor* Card)
{
	if (Card->GetCardInfo().CardType >= ECardType::MagicalUnicorn)
	{
		PlayerBoards.Find(PlayerIndex)->StableUnicorns.Remove(Card);
	}
	else
	{
		PlayerBoards.Find(PlayerIndex)->StableEffects.Remove(Card);
	}
	CopyOfStables.Remove(Card);
	OnPlayerStableChanged.Broadcast(PlayerIndex, Card, true);
	Card->OnLeaveStable();
}

void UUnicornCardGameManager::MoveToPlayerStable(int32 PlayerIndex, AUnicornCardActor* Card)
{
	for (int32 i = 1; i <= UUnicornSettings::GetUnicornSettings()->NumberOfPlayers; i++)
	{
		if (PlayerBoards.Find(i)->StableUnicorns.Contains(Card))
		{
			RemoveFromPlayerStable(i, Card);
			AddToPlayerStable(PlayerIndex, Card);
			return;
		}
	}
}

void UUnicornCardGameManager::NextPhase()
{
	switch (CurrentTurnPhase)
	{
	case Beginning:
		SetPhase(ETurnPhase::Draw);
		break;
	case Draw:
		SetPhase(ETurnPhase::Action);
		break;
	case Action:
		SetPhase(ETurnPhase::End);
		break;
	case End:
		for (int32 i = 1; i <= UUnicornSettings::GetUnicornSettings()->NumberOfPlayers; i++)
		{
			if (PlayerBoards.Find(i)->StableUnicorns.Num() >= 7)
			{
				//WIN
				OnWinFinished.Broadcast(i);
				return;
			}
		}
		if (PlayerBoards.Find(CurrentPlayerIndex)->Hand.Num() > UUnicornSettings::GetUnicornSettings()->NumberOfCardsMaxInHand)
		{
			//Discard
			OnDiscardRequired.Broadcast(CurrentPlayerIndex, false);
			return;
		}
		NextPlayer();
		SetPhase(ETurnPhase::Beginning);
		break;
	}	
}

void UUnicornCardGameManager::NextPlayer()
{
	CurrentPlayerIndex++;
	if (CurrentPlayerIndex > PlayerBoards.Num())
	{
		CurrentPlayerIndex = 1;
	}
	OnPlayerTurnChanged.Broadcast(CurrentPlayerIndex);
}

void UUnicornCardGameManager::InitManager()
{
	UUnicornSettings::GetUnicornSettings()->CardsTable.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateLambda([WeakThis = TWeakObjectPtr<UUnicornCardGameManager>(this)](const FSoftObjectPath& InSoftObjectPath, UObject* InObject)
	{
		UDataTable* Asset = Cast<UDataTable>(InObject);
		if(Asset == nullptr)
		{
			return;
		}
		TStrongObjectPtr<UUnicornCardGameManager> PinnedVariablesOutliner = WeakThis.Pin();
		if(!PinnedVariablesOutliner.IsValid())
		{
			return;
		}

		TArray<FDeckCard*> Rows;
		const FString ContextString = TEXT("UUnicornCardGameManager::Init()");
		Asset->GetAllRows<FDeckCard>(ContextString, Rows);
		for (FDeckCard*	Card : Rows)
		{
			//TODO:: load async
			for (int32 CardAmount = 0; CardAmount < Card->CardAmount; CardAmount++)
			{
				AUnicornCardActor* NewActor = PinnedVariablesOutliner.Get()->GetWorld()->SpawnActor<AUnicornCardActor>(Card->Card.LoadSynchronous());
				if (NewActor != nullptr)
				{
					if (NewActor->GetCardInfo().CardType == ECardType::BabyUnicorn)
					{
						PinnedVariablesOutliner->AddToNursery(NewActor);
					}
					else
					{
						PinnedVariablesOutliner->AddToDrawPile(NewActor);
					}
				}
			}
		}
		if (PinnedVariablesOutliner->DrawPile.Num() == 0)
		{
			return;
		}
		FRandomStream RandomStream(0);
		PinnedVariablesOutliner->ManualShuffle(PinnedVariablesOutliner->DrawPile, RandomStream);
		PinnedVariablesOutliner->ManualShuffle(PinnedVariablesOutliner->Nursery, RandomStream);
		//init player boards
		for (int32 i = 1; i <= UUnicornSettings::GetUnicornSettings()->NumberOfPlayers; i++)
		{
			FPlayerCardBoard PlayerCardBoard;
			PinnedVariablesOutliner->PlayerBoards.Add(i, PlayerCardBoard);
			AUnicornCardActor* LastBaby = PinnedVariablesOutliner->Nursery.Last();
			if (LastBaby != nullptr)
			{
				PinnedVariablesOutliner->AddToPlayerStable(i, LastBaby);
				PinnedVariablesOutliner->RemoveFromNursery(LastBaby);
			}
			for (int32 j = 1; j <= UUnicornSettings::GetUnicornSettings()->NumberOfCardsToStartWith; j++)
			{
				PinnedVariablesOutliner->GiveCardToPlayerFromDrawPile(i);
			}
		}
		PinnedVariablesOutliner->SetPhase(ETurnPhase::Action);
	}));
	
	OnEffectCard.AddUniqueDynamic(this, &UUnicornCardGameManager::OnEffectPlayed);
}

void UUnicornCardGameManager::ManualShuffle(TArray<AUnicornCardActor*>& Array, FRandomStream& Stream)
{
	for (int32 i = Array.Num() - 1; i > 0; --i)
	{
		int32 Index = Stream.RandRange(0, i);
		if (i != Index)
		{
			Array.Swap(i, Index);
		}
	}
	//might need an event to tell the entire pile changed . e.g. for discard pile
}

void UUnicornCardGameManager::GiveCardToPlayerFromDrawPile(int32 PlayerIndex)
{
	if (DrawPile.Num() == 0)
	{
		if (DiscardPile.Num() == 0)
		{
			return;
		}
		for (int32 i = 0; i < DiscardPile.Num(); i++)
		{
			//not a fan. rethink
			AddToDrawPile(DiscardPile[i]);
			RemoveFromDiscardPile(DiscardPile[i]);
			//Redo the random stream in a diff way. It's giving same results each time.
			FRandomStream RandomStream(6);
			ManualShuffle(DrawPile, RandomStream);
		}
	}
	AUnicornCardActor* LastCard = DrawPile.Last();
	if (LastCard != nullptr)
	{
		AddToPlayerHand(PlayerIndex, LastCard);
		RemoveFromDrawPile(LastCard);
	}
}

bool UUnicornCardGameManager::IsCardPartOfPlayersHand(int32 PlayerIndex, AUnicornCardActor* Card)
{
	if (PlayerBoards.Find(PlayerIndex) == nullptr)
	{
		return false;
	}
	TArray<AUnicornCardActor*> Hand = PlayerBoards[PlayerIndex].Hand;
	bool Found = Hand.Contains(Card);
	return Found;
}

bool UUnicornCardGameManager::IsCardPartOfPlayersStable(int32 PlayerIndex, AUnicornCardActor* Card)
{
	if (PlayerBoards.Find(PlayerIndex) == nullptr)
	{
		return false;
	}
	TArray<AUnicornCardActor*> StableUnicorns = PlayerBoards[PlayerIndex].StableUnicorns;
	bool Found = StableUnicorns.Contains(Card);
	return Found;
}

void UUnicornCardGameManager::ExecuteCardEffect(EEffectWord Effect, int32 PlayerIndex, AUnicornCardActor* Card)
{
	//TODO: confirm we have card to be removed as this is called by UI/client and we can't trust it
	switch (Effect) {
	case None:
		break;
	case Sacrifice:// move a card from your stable to the discard pile
	case Destroy://Move a card from any other player's stable to the discard pile
		RemoveFromPlayerStable(PlayerIndex, Card);
		AddToDiscardPile(Card);
		break;
	case Steal://Move a card from any other player's stable into your stable
		RemoveFromPlayerStable(PlayerIndex, Card);
		AddToPlayerStable(CurrentPlayerIndex, Card);
		break;
	case Discard://Move a card from your hand to the discard pile
		RemoveFromPlayerHand(PlayerIndex, Card);
		AddToDiscardPile(Card);
		NextPhase();//should we do this here?
		break;
	case Revert://Move a card from any other player's stable into their hand
		RemoveFromPlayerStable(PlayerIndex, Card);
		AddToPlayerHand(PlayerIndex, Card);
		break;
	}
}

void UUnicornCardGameManager::SetPhase(const ETurnPhase NewPhase)
{
	OnPhaseAboutToBeChanged.Broadcast(NewPhase);
	if (!GetWorld())
	{
		DoPhaseChange(NewPhase);
		return;
	}
	if (GetWorld()->GetTimerManager().IsTimerActive(DelayedPhaseChangeTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(DelayedPhaseChangeTimerHandle);
	}
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("DoPhaseChange"), NewPhase);
	GetWorld()->GetTimerManager().SetTimer(DelayedPhaseChangeTimerHandle, TimerDelegate, 1.5f, false);
}


void UUnicornCardGameManager::DoPhaseChange(const ETurnPhase NewPhase)
{	
	CurrentTurnPhase = NewPhase;
	OnPhaseChanged.Broadcast(NewPhase);
	DoPhaseLogic();
}

void UUnicornCardGameManager::DoPhaseLogic()
{
	switch (CurrentTurnPhase)
	{
	case Beginning:
		for (AUnicornCardActor* Actor : CopyOfStables)
		{
			if (!Actor->OnBeginPhase())
			{
				return;
			}
		}
		break;
	case Draw:
		for (AUnicornCardActor* Actor : CopyOfStables)
		{
			Actor->OnDrawPhase();
		}
		GiveCardToPlayerFromDrawPile(CurrentPlayerIndex);
		break;
	case Action:
		for (AUnicornCardActor* Actor : CopyOfStables)
		{
			Actor->OnActionPhase();
		}
		return;
	case End:
		for (AUnicornCardActor* Actor : CopyOfStables)
		{
			if (!Actor->OnEndPhase())
			{
				return;
			}
		}
		break;
	}
	NextPhase();
}

void UUnicornCardGameManager::OnEffectPlayed(int32 PlayerIndex, EEffectWord Effect, bool bOptional)
{
	if (Effect == EEffectWord::None)
	{
		return;
	}

	OnEffectEventPlayed.Broadcast(Effect, PlayerIndex, bOptional);
}

