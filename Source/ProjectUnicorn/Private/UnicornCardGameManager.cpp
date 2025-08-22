// Fill out your copyright notice in the Description page of Project Settings.


#include "UnicornCardGameManager.h"
#include "UnicornCardActor.h"
#include "UnicornSettings.h"

void UUnicornCardGameManager::AddToDrawPile(AUnicornCardActor* Card)
{
	if (Card->GetCardInfo().CardType == ECardType::BabyUnicorn)
	{
		AddToNursery(Card);
		return;
	}
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
	if (Card->GetCardInfo().CardType == ECardType::BabyUnicorn)
	{
		AddToNursery(Card);
		return;
	}
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
	if (Card->GetCardInfo().CardType == ECardType::BabyUnicorn)
	{
		AddToNursery(Card);
		return;
	}
	Card->SetOwningPlayer(PlayerIndex);
	PlayerBoards.Find(PlayerIndex)->Hand.Add(Card);	
	OnPlayerHandChanged.Broadcast(PlayerIndex, Card, false);
	Card->OnEnterHand();
}

void UUnicornCardGameManager::RemoveFromPlayerHand(int32 PlayerIndex, AUnicornCardActor* Card)
{
	Card->SetOwningPlayer(-1);
	PlayerBoards.Find(PlayerIndex)->Hand.Remove(Card);	
	OnPlayerHandChanged.Broadcast(PlayerIndex, Card, true);
}

void UUnicornCardGameManager::AddToPlayerStable(int32 PlayerIndex, AUnicornCardActor* Card)
{
	Card->SetOwningPlayer(PlayerIndex);
	if (Card->GetCardInfo().CardType >= ECardType::MagicalUnicorn)
	{
		PlayerBoards.Find(PlayerIndex)->StableUnicorns.Add(Card);
	}
	else
	{
		PlayerBoards.Find(PlayerIndex)->StableEffects.Add(Card);
	}
	//adding only if we are currently processing the stable cards
	if (CopyOfStables.Num() > 0)
	{
		CopyOfStables.AddUnique(Card);
	}
	OnPlayerStableChanged.Broadcast(PlayerIndex, Card, false);
	Card->OnEnterStable();
}

void UUnicornCardGameManager::RemoveFromPlayerStable(int32 PlayerIndex, AUnicornCardActor* Card)
{
	Card->SetOwningPlayer(-1);
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
	//if we have stuff to process we should continue with that
	if (CopyOfStables.Num() > 0)
	{
		return;
	}
	if (CurrentEffect.Value > EEffectWord::None)
	{
		return;
	}
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
		if (CheckForWinCondition())
		{
			return;
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
					NewActor->SetManager(PinnedVariablesOutliner.Get());
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
		PinnedVariablesOutliner->ManualShuffle(PinnedVariablesOutliner->DrawPile);
		PinnedVariablesOutliner->ManualShuffle(PinnedVariablesOutliner->Nursery);
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
}

void UUnicornCardGameManager::ManualShuffle(TArray<AUnicornCardActor*>& Array)
{
	for (int32 i = Array.Num() - 1; i > 0; --i)
	{
		const int32 Index = FMath::RandRange(0, i);
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
		}
		ManualShuffle(DrawPile);
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
		RemoveFromPlayerStable(PlayerIndex, Card);
		AddToDiscardPile(Card);
		//NextPhase();//should we do this here?
		break;
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
		//NextPhase();//should we do this here?
		break;
	case Revert://Move a card from any other player's stable into their hand
		RemoveFromPlayerStable(PlayerIndex, Card);
		AddToPlayerHand(PlayerIndex, Card);
		break;
	}
	CurrentEffect.Key = false;
	CurrentEffect.Value = EEffectWord::None;
	OnEffectEventExecuted.Broadcast(Effect, PlayerIndex, true);
}

void UUnicornCardGameManager::AddCardTypeFromHandToStable(ECardType Type, int32 PlayerIndex)
{
	TArray<AUnicornCardActor*> Cards = GetPlayerHand(PlayerIndex);
	for (int32 i = 0; i < Cards.Num(); i++)
	{
		if (Cards[i]->GetCardInfo().CardType == Type)
		{
			RemoveFromPlayerHand(PlayerIndex, Cards[i]);
			AddToPlayerStable(PlayerIndex, Cards[i]);
			return;
		}
	}
}

void UUnicornCardGameManager::EndEffectTurn(int32 PlayerIndex, EEffectWord Effect, AUnicornCardActor* Card)
{	
	CurrentEffect.Key = false;
	CurrentEffect.Value = EEffectWord::None;
	OnCardEffectOptionalEnded.Broadcast();
}

void UUnicornCardGameManager::ExecutePlayCard(AUnicornCardActor* Card)
{
	TArray<int32> Players;
	PlayerBoards.GetKeys(Players);
	//TODO:: Add a way to choose opponent for downgrade
	int32 TargetPlayer = -1;
	for (int32 i = 0; i < Players.Num(); i++)
	{
		if (Players[i] != CurrentPlayerIndex)
		{
			TargetPlayer = Players[i];
			break;
		}
	}
	switch (Card->GetCardInfo().CardType)
	{
	case Instant:
		break;
	case Upgrade:
	case MagicalUnicorn:
	case BasicUnicorn:
	case BabyUnicorn:
		RemoveFromPlayerHand(CurrentPlayerIndex, Card);
		AddToPlayerStable(CurrentPlayerIndex, Card);
		break;
	case Downgrade:
		RemoveFromPlayerHand(CurrentPlayerIndex, Card);		
		if (TargetPlayer != CurrentPlayerIndex && TargetPlayer != -1)
		{
			AddToPlayerStable(TargetPlayer, Card);
		}
		break;
	case Magic:
		Card->OnPlayedCard();
		RemoveFromPlayerHand(CurrentPlayerIndex, Card);
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
	OnCardPhaseCompleted.RemoveDynamic(this, &UUnicornCardGameManager::DoPhaseLogicForFirstCard);
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("DoPhaseChange"), NewPhase);
	GetWorld()->GetTimerManager().SetTimer(DelayedPhaseChangeTimerHandle, TimerDelegate, 1.5f, false);
}


void UUnicornCardGameManager::DoPhaseChange(const ETurnPhase NewPhase)
{	
	CurrentTurnPhase = NewPhase;
	OnPhaseChanged.Broadcast(NewPhase);
	//we have to trigger each card's phase and wait on a response to move onto next one
	CopyOfStables.Empty();
	for (int32 i = 1; i <= UUnicornSettings::GetUnicornSettings()->NumberOfPlayers; i++)
	{
		for (AUnicornCardActor* UnicornCard: PlayerBoards.Find(i)->StableUnicorns)
		{
			CopyOfStables.AddUnique(UnicornCard);
		}
		for (AUnicornCardActor* EffectCard: PlayerBoards.Find(i)->StableEffects)
		{
			CopyOfStables.AddUnique(EffectCard);
		}
	}	
	
	OnCardPhaseCompleted.AddUniqueDynamic(this, &UUnicornCardGameManager::DoPhaseLogicForFirstCard);
	DoPhaseLogicForFirstCard();
}

void UUnicornCardGameManager::DoPhaseLogicForFirstCard()
{
	if (CheckForWinCondition())
	{
		return;
	}
	if (CopyOfStables.Num() == 0)
	{
		OnCardPhaseCompleted.RemoveDynamic(this, &UUnicornCardGameManager::DoPhaseLogicForFirstCard);		
		switch (CurrentTurnPhase)
		{
		case Beginning:
			break;
		case Draw:
			GiveCardToPlayerFromDrawPile(CurrentPlayerIndex);
			break;
		case Action:
			return;
		case End:
			break;
		}
		NextPhase();
		return;
	}
	AUnicornCardActor* FirstCard = CopyOfStables[0];
	CopyOfStables.RemoveAt(0);
	switch (CurrentTurnPhase)
	{
	case Beginning:
		FirstCard->OnBeginPhase();
		break;
	case Draw:
		FirstCard->OnDrawPhase();
		break;
	case Action:
		FirstCard->OnActionPhase();
		return;
	case End:
		FirstCard->OnEndPhase();
		break;
	}
}

bool UUnicornCardGameManager::CheckForWinCondition()
{
	for (int32 i = 1; i <= UUnicornSettings::GetUnicornSettings()->NumberOfPlayers; i++)
	{
		if (PlayerBoards.Find(i)->StableUnicorns.Num() >= 7)
		{
			//WIN
			OnWinFinished.Broadcast(i);
			return true;
		}
	}
	return false;
}

bool UUnicornCardGameManager::InvokeEffect(const int32 PlayerIndex, const EEffectWord Effect, const bool bOptional)
{
	if (Effect == EEffectWord::None)
	{
		return false;
	}
	if (!CanInvokeEffect(PlayerIndex, Effect))
	{
		return false;
	}
	CurrentEffect.Key = bOptional;
	CurrentEffect.Value = Effect;

	OnEffectUIEventPlayed.Broadcast(Effect, PlayerIndex, bOptional);
	return true;
}

bool UUnicornCardGameManager::CanInvokeEffect(int32 PlayerIndex, EEffectWord Effect)
{	
	bool bCanContinue = false;
	//needs more fleshing out
	TArray<AUnicornCardActor*> AllUnicorns = GetPlayerStableUnicorns(PlayerIndex);
	for (AUnicornCardActor* Unicorn : AllUnicorns)
	{
		if (Unicorn->CanEffectBePlayedOnSelf(Effect))
		{				
			bCanContinue = true;
			break;
		}
	}
	TArray<AUnicornCardActor*> AllEffects = GetPlayerStableEffects(PlayerIndex);
	for (AUnicornCardActor* EffectCard : AllEffects)
	{
		if (EffectCard->CanEffectBePlayedOnSelf(Effect))
		{				
			bCanContinue = true;
			break;
		}
	}
	if (!bCanContinue)
	{
		return false;
	}
	return true;
}

