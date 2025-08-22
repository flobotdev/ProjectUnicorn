// Fill out your copyright notice in the Description page of Project Settings.


#include "UnicornCardActor.h"

#include "UnicornCardGameManager.h"

AUnicornCardActor::AUnicornCardActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AUnicornCardActor::CanPlayCard_Implementation()
{
	if (!CardManager)
	{
		return false;
	}
	return CardManager->CanInvokeEffect(CurrentOwningPlayer, CardInfo.Effect);
}

bool AUnicornCardActor::CanEffectBePlayedOnSelf(const EEffectWord Effect)
{
	switch (Effect)
	{
		case Sacrifice:
			return bCanBeSacrificed;
		case EEffectWord::Destroy:
			return bCanBeDestroyed;
		case Steal:
		case Discard:
		case Revert:
			return bCanBeAffectedByMagic;
		case None:
			break;
	}
	return true;
}

bool AUnicornCardActor::CanPlayEffects_Implementation()
{
	return bCanPlayEffects;
}

bool AUnicornCardActor::CanEnterStable_Implementation()
{
	return bCanEnterStable;
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

