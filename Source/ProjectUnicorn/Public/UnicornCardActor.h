// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SharedStructs.h"
#include "GameFramework/Actor.h"
#include "UnicornCardActor.generated.h"

class UUnicornCardGameManager;

UCLASS()
class PROJECTUNICORN_API AUnicornCardActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUnicornCardActor();
	void SetManager(UUnicornCardGameManager* Manager) { CardManager = Manager; }
	void SetOwningPlayer(const int32 OwningPlayer) { CurrentOwningPlayer = OwningPlayer; }
	UFUNCTION(BlueprintNativeEvent)
	bool OnBeginPhase();
	UFUNCTION(BlueprintNativeEvent)
	void OnDrawPhase();
	UFUNCTION(BlueprintNativeEvent)
	void OnActionPhase();
	UFUNCTION(BlueprintNativeEvent)
	bool OnEndPhase();
	UFUNCTION(BlueprintNativeEvent)
	void OnEnterStable();
	UFUNCTION(BlueprintNativeEvent)
	void OnLeaveStable();
	UFUNCTION(BlueprintNativeEvent)
	void OnSacrificed();
	UFUNCTION(BlueprintNativeEvent)
	void OnCardDestroyed();
	UFUNCTION(BlueprintNativeEvent)
	void OnEnterHand();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPlayedCard();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanEnterStable();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanPlayCard();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanPlayEffects();
	UFUNCTION(BlueprintCallable)
	void SetCanEnterStable(const bool bCanEnter){ bCanEnterStable = bCanEnter; }
	UFUNCTION(BlueprintCallable)
	void SetCanPlayEffects(const bool bCanPlay){ bCanPlayEffects = bCanPlay; }
	UFUNCTION(BlueprintCallable)
	bool CanEffectBePlayedOnSelf(const EEffectWord Effect);


	UFUNCTION(BlueprintCallable, BlueprintPure)
	FUnicornCardInfo GetCardInfo() { return CardInfo; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Unicorn, meta = (AllowPrivateAccess = "true"))
	bool bCanBeDestroyed = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Unicorn, meta = (AllowPrivateAccess = "true"))
	bool bCanBeSacrificed = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Unicorn, meta = (AllowPrivateAccess = "true"))
	bool bCanBeAffectedByMagic = true;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Unicorn, meta = (AllowPrivateAccess = "true"))
	FUnicornCardInfo CardInfo;
	UPROPERTY()
	bool bCanEnterStable = true;
	UPROPERTY()
	bool bCanPlayEffects = true;
	UPROPERTY(BlueprintReadOnly, Category = Manager, meta = (AllowPrivateAccess = "true"))
	UUnicornCardGameManager* CardManager = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = Manager, meta = (AllowPrivateAccess = "true"))
	int32 CurrentOwningPlayer = -1;
};


USTRUCT(BlueprintType)
struct FDeckCard : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AUnicornCardActor> Card = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=1, ClampMax=100, UIMin=1, UIMax=100))
	int32 CardAmount = 1;
};
