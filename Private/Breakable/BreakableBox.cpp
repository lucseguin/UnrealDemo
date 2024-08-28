// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableBox.h"
#include "Components/BoxComponent.h"

ABreakableBox::ABreakableBox() : ABreakableBase() {
    BlockingShape = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    BlockingShape->SetupAttachment(GetRootComponent());
    BlockingShape->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BlockingShape->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}
