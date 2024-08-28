// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableCapsule.h"
#include "Components/CapsuleComponent.h"

ABreakableCapsule::ABreakableCapsule() : ABreakableBase()
{
    BlockingShape = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    BlockingShape->SetupAttachment(GetRootComponent());
    BlockingShape->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BlockingShape->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}
