// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SharedStructs.generated.h"

UENUM(BlueprintType)
enum ECardType : uint8
{
	Instant,
	Upgrade,
	Downgrade,
	Magic,
	MagicalUnicorn,
	BasicUnicorn,
	BabyUnicorn
};

UENUM(BlueprintType)
enum ETurnPhase : uint8
{
	Beginning, //do any card effects from stable
	Draw, //draw one card into hand from draw pile
	Action, //play one card or draw another
	End //discard to limit from hand
};


UENUM(BlueprintType)
enum EEffectWord : uint8
{
	None,
	Sacrifice, // move a card from your stable to the discard pile
	Destroy, //Move a card from any other player's stable to the discard pile
	Steal, //Move a card from any other player's stable into your stable
	Discard, //Move a card from your hand to the discard pile
	Revert //Move a card from any other player's stable into their hand
};

USTRUCT(BlueprintType)
struct FCardTypeInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor BackgroundColor = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> IconImage = nullptr;
};


USTRUCT(BlueprintType)
struct FCardEffect
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETurnPhase> Phase = ETurnPhase::Beginning;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EEffectWord> Effect = EEffectWord::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OptionalEffect = false;
};

USTRUCT(BlueprintType)
struct FUnicornCardInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECardType> CardType = ECardType::Instant;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CardTitle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CardDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EEffectWord> Effect = EEffectWord::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> CardImage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor ImageTint = FLinearColor::White;
};
