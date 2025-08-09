// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SharedStructs.h"
#include "UObject/Object.h"
#include "UnicornCardGameManager.generated.h"

class AUnicornCardActor;

USTRUCT(BlueprintType)
struct FPlayerCardBoard
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AUnicornCardActor*> StableUnicorns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AUnicornCardActor*> StableEffects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AUnicornCardActor*> Hand;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, ETurnPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDrawPileChanged, AUnicornCardActor*, Card, bool, bRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDiscardPileChanged, AUnicornCardActor*, Card, bool, bRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNurseryPileChanged, AUnicornCardActor*, Card, bool, bRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerHandChanged, int32, PlayerIndex, AUnicornCardActor*, Card, bool, bRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerStableChanged, int32, PlayerIndex, AUnicornCardActor*, Card, bool, bRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerTurnChanged, int32, PlayerIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDiscardRequired, int32, PlayerIndex, bool, bOptionalDiscard);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCardPhaseCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCardEffectOptionalEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWinFinished, int32, PlayerIndex);
//this is for UI to listen to
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEffectPlayed, EEffectWord, Effect, int32, PlayerIndex, bool, bOptional);
UCLASS(BlueprintType)
class PROJECTUNICORN_API UUnicornCardGameManager : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AUnicornCardActor*> GetDrawPile() const { return DrawPile; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AUnicornCardActor*> GetDiscardPile() const { return DiscardPile; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AUnicornCardActor*> GetNursery() const { return Nursery; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AUnicornCardActor*> GetPlayerHand(int32 PlayerIndex) const { return PlayerBoards[PlayerIndex].Hand; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AUnicornCardActor*> GetPlayerStableUnicorns(int32 PlayerIndex) const
	{
		if (!PlayerBoards.Contains(PlayerIndex))
		{
			return TArray<AUnicornCardActor*>();
		}
		return PlayerBoards[PlayerIndex].StableUnicorns;
	}
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AUnicornCardActor*> GetPlayerStableEffects(int32 PlayerIndex) const
	{
		if (!PlayerBoards.Contains(PlayerIndex))
		{
			return TArray<AUnicornCardActor*>();
		}
		return PlayerBoards[PlayerIndex].StableEffects;
	}
	UFUNCTION(BlueprintCallable)
	void AddToDrawPile(AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void RemoveFromDrawPile(AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void AddToDiscardPile(AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void RemoveFromDiscardPile(AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void AddToNursery(AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void RemoveFromNursery(AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void AddToPlayerHand(int32 PlayerIndex, AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void RemoveFromPlayerHand(int32 PlayerIndex, AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void AddToPlayerStable(int32 PlayerIndex, AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void RemoveFromPlayerStable(int32 PlayerIndex, AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void MoveToPlayerStable(int32 PlayerIndex, AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void NextPhase();
	UFUNCTION(BlueprintCallable)
	void NextPlayer();
	UFUNCTION(BlueprintCallable)
	void SetPhase(const ETurnPhase NewPhase);
	UFUNCTION(BlueprintCallable)
	void InitManager();
	UFUNCTION(BlueprintCallable)
	void GiveCardToPlayerFromDrawPile(int32 PlayerIndex);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentPlayerIndex() const { return CurrentPlayerIndex; }
	UFUNCTION(BlueprintCallable)
	bool IsCardPartOfPlayersHand(int32 PlayerIndex, AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	bool IsCardPartOfPlayersStable(int32 PlayerIndex, AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void ExecuteCardEffect(EEffectWord Effect, int32 PlayerIndex, AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void AddCardTypeFromHandToStable(ECardType Type, int32 PlayerIndex);
	UFUNCTION(BlueprintCallable)
	void EndEffectTurn(int32 PlayerIndex, EEffectWord Effect, AUnicornCardActor* Card);
	UFUNCTION(BlueprintCallable)
	void ExecutePlayCard(AUnicornCardActor* Card);

	//events
	UPROPERTY(BlueprintAssignable)
	FOnPhaseChanged OnPhaseChanged;
	UPROPERTY(BlueprintAssignable)
	FOnPhaseChanged OnPhaseAboutToBeChanged;
	UPROPERTY(BlueprintAssignable)
	FOnDrawPileChanged OnDrawPileChanged;
	UPROPERTY(BlueprintAssignable)
	FOnDiscardPileChanged OnDiscardPileChanged;
	UPROPERTY(BlueprintAssignable)
	FOnNurseryPileChanged OnNurseryPileChanged;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerHandChanged OnPlayerHandChanged;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStableChanged OnPlayerStableChanged;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerTurnChanged OnPlayerTurnChanged;
	UPROPERTY(BlueprintAssignable)
	FOnDiscardRequired OnDiscardRequired;
	UPROPERTY(BlueprintAssignable)
	FOnWinFinished OnWinFinished;

	UFUNCTION(BlueprintCallable)
	bool InvokeEffect(int32 PlayerIndex, EEffectWord Effect, bool bOptional);
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnCardPhaseCompleted OnCardPhaseCompleted;	
	UPROPERTY(BlueprintAssignable)
	FOnEffectPlayed OnEffectUIEventPlayed;
	UPROPERTY(BlueprintAssignable)
	FOnEffectPlayed OnEffectEventExecuted;
	UPROPERTY(BlueprintAssignable)
	FOnCardEffectOptionalEnded OnCardEffectOptionalEnded;
	
	
protected:
	UPROPERTY()
	TArray<AUnicornCardActor*> DiscardPile; //TODO:: Does it need actors or can we have just soft classes?
	UPROPERTY()
	TArray<AUnicornCardActor*> DrawPile; //TODO:: Does it need actors or can we have just soft classes?
	UPROPERTY()
	TArray<AUnicornCardActor*> Nursery;
	UPROPERTY()
	TArray<AUnicornCardActor*> CopyOfStables;
	
	TMap<int32, FPlayerCardBoard> PlayerBoards;

	TEnumAsByte<ETurnPhase> CurrentTurnPhase;
	int32 CurrentPlayerIndex = 1;
	TTuple<bool, EEffectWord> CurrentEffect;


	void ManualShuffle(TArray<AUnicornCardActor*>& Array);
	UFUNCTION()
	void DoPhaseLogicForFirstCard();

	bool CheckForWinCondition();

	UFUNCTION()
	void DoPhaseChange(const ETurnPhase NewPhase);
	
	FTimerHandle DelayedPhaseChangeTimerHandle;
};
