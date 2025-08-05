// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SharedStructs.h"
#include "GameFramework/Actor.h"
#include "UnicornCardActor.generated.h"

UCLASS()
class PROJECTUNICORN_API AUnicornCardActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUnicornCardActor();
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


	UFUNCTION(BlueprintCallable, BlueprintPure)
	FUnicornCardInfo GetCardInfo() { return CardInfo; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Unicorn, meta = (AllowPrivateAccess = "true"))
	FUnicornCardInfo CardInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Unicorn, meta = (AllowPrivateAccess = "true"))
	bool bCanBeDestroyed = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Unicorn, meta = (AllowPrivateAccess = "true"))
	bool bCanBeSacrificed = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Unicorn, meta = (AllowPrivateAccess = "true"))
	bool bCanBeAffectedByMagic = true;
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
