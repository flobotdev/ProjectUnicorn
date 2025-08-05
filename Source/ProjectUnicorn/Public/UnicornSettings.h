// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UnicornSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, DefaultConfig, meta = (DisplayName = "Unicorn Default Settings"))
class PROJECTUNICORN_API UUnicornSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UDataTable> CardsTable;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	int32 NumberOfPlayers = 2;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	int32 NumberOfCardsToStartWith = 3;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	int32 NumberOfCardsForWin = 7;
	UPROPERTY(config, EditAnywhere, BlueprintReadOnly)
	int32 NumberOfCardsMaxInHand = 7;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Unicorn Settings")
	static const UUnicornSettings* GetUnicornSettings() { return GetDefault<UUnicornSettings>();};
};
