// Fill out your copyright notice in the Description page of Project Settings.


#include "UnicornCardActor.h"

AUnicornCardActor::AUnicornCardActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AUnicornCardActor::OnPlayedCard_Implementation()
{
}

void AUnicornCardActor::OnCardDestroyed_Implementation()
{
}

void AUnicornCardActor::OnEnterHand_Implementation()
{
}

bool AUnicornCardActor::OnBeginPhase_Implementation()
{
	return true;
}

void AUnicornCardActor::OnDrawPhase_Implementation()
{
}

void AUnicornCardActor::OnActionPhase_Implementation()
{
}

bool AUnicornCardActor::OnEndPhase_Implementation()
{
	return true;
}

void AUnicornCardActor::OnEnterStable_Implementation()
{
}

void AUnicornCardActor::OnLeaveStable_Implementation()
{
}

void AUnicornCardActor::OnSacrificed_Implementation()
{
}

