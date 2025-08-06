// Fill out your copyright notice in the Description page of Project Settings.


#include "UnicornCardActor.h"

#include "UnicornCardGameManager.h"

AUnicornCardActor::AUnicornCardActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AUnicornCardActor::CanEnterStable_Implementation()
{
	return true;
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
	CardManager->OnCardPhaseCompleted.Broadcast();
	return true;
}

void AUnicornCardActor::OnDrawPhase_Implementation()
{
	CardManager->OnCardPhaseCompleted.Broadcast();
}

void AUnicornCardActor::OnActionPhase_Implementation()
{
	CardManager->OnCardPhaseCompleted.Broadcast();
}

bool AUnicornCardActor::OnEndPhase_Implementation()
{
	CardManager->OnCardPhaseCompleted.Broadcast();
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

