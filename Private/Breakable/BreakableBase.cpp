// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableBase.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ABreakableBase::ABreakableBase()
{
     // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Collection"));
    SetRootComponent(GeometryCollection);
    
    GeometryCollection->SetGenerateOverlapEvents(true);
    GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    GeometryCollection->SetNotifyBreaks(true);
}

void ABreakableBase::BeginPlay()
{
    Super::BeginPlay();
    
    GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableBase::OnChaosBreakEvent);
}


void ABreakableBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABreakableBase::ProcessHit(const FVector& ImpactPoint)
{
    if (!bBroken)
    {
        bBroken = true;
        
        if(BreakingSound)
            UGameplayStatics::PlaySoundAtLocation(this, BreakingSound, ImpactPoint);
        
        if(BlockingShape)
            BlockingShape->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        
        UWorld* World = GetWorld();
        if(World && TreasureClasses.Num() > 0)
        {
            FVector Location = GetActorLocation();
            Location.Z += 75.f;
            World->SpawnActor<ATreasure>(TreasureClasses[FMath::RandRange(0, TreasureClasses.Num()-1)], Location, GetActorRotation());
        }
        
        SetLifeSpan(FMath::RandRange(2, 4));
    }
}

void ABreakableBase::OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent)
{
    ProcessHit(BreakEvent.Location);
}

void ABreakableBase::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    ProcessHit(ImpactPoint);
}

