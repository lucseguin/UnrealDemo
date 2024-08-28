// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
    WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
    WeaponBox->SetupAttachment(GetRootComponent());
    WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    
    BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    BoxTraceStart->SetupAttachment(GetRootComponent());
    
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    
    WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    if (IsSameActorType(OtherActor)) return; 

    FHitResult BoxHit;
    BoxTrace(BoxHit);

    if(BoxHit.GetActor())
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *BoxHit.GetActor()->GetName());

        if (IsSameActorType(BoxHit.GetActor())) return; 

        //apply damage first to actor
        UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
        ExecuteGetHit(BoxHit);
        CreateFields(BoxHit.ImpactPoint);
    }
}

bool AWeapon::IsSameActorType(AActor* OtherActor)
{
    return (GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy")));
}
void AWeapon::ExecuteGetHit(const FHitResult& BoxHit)
{
    IHitInterface* HitActor = Cast<IHitInterface>(BoxHit.GetActor());
    if(HitActor) {
        HitActor->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner() );
    }
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
    const FVector Start = BoxTraceStart->GetComponentLocation();
    const FVector End = BoxTraceEnd->GetComponentLocation();
    
    TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

    for(AActor* Actor : IgnoreActors)
    {
        ActorsToIgnore.AddUnique(Actor);
    }
    
    // for(AActor* Actor : ActorsToIgnore)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("BoxTrace - Will ignore Actor: %s"), *Actor->GetName());
    // }

    UKismetSystemLibrary::BoxTraceSingle(
                                         this,
                                         Start,
                                         End,
                                         BoxTraceExtent,
                                         BoxTraceStart->GetComponentRotation(),
                                         ETraceTypeQuery::TraceTypeQuery1,
                                         false,
                                         ActorsToIgnore,
                                         bShowBoxDebug?EDrawDebugTrace::Type::ForDuration:EDrawDebugTrace::Type::None,
                                         BoxHit,
                                         true,
                                         FColor::Green, 
                                         FColor::Red,
                                         3.0f);
    IgnoreActors.AddUnique(BoxHit.GetActor()); // as the box trace move along the enemy, we only want to have a single hit on that character
}

void AWeapon::AttachMeshToSocket(USceneComponent *InParent, FName InSocketName) {
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, AttachmentRules, InSocketName);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
    ItemState = EItemState::EIS_Equipped;
    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    AttachMeshToSocket(InParent, InSocketName);
    DeactivateEmbers();
    PlayEquipSound();
    DisableSphereCollision();
}

void AWeapon::Unequip()
{
    SetOwner(nullptr);
    SetInstigator(nullptr);
    Destroy();
}
void AWeapon::PlayEquipSound()
{
    if(EquipSound)
        UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
}

void AWeapon::DisableSphereCollision()
{
    if(Sphere)
        Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void AWeapon::DeactivateEmbers()
{
    if(EmbersEffect)
        EmbersEffect->Deactivate();
}
