// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "DebugMacros.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

    PawnSensingCmp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
    PawnSensingCmp->SetPeripheralVisionAngle(PeripheralVisonAngle);
    PawnSensingCmp->SightRadius = 4000.f;
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if(EquippedWeapon && EquippedWeapon->GetWeaponBox())
    {
        EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
        EquippedWeapon->IgnoreActors.Empty();
    }
} 

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    if(PawnSensingCmp)
        PawnSensingCmp->OnSeePawn.AddDynamic(this, &ABaseCharacter::PawnSeenImpl);    
}

void ABaseCharacter::PawnSeenImpl(APawn *SeenPawn)
{
    //UE_LOG(LogTemp, Warning, TEXT("[%s]  ABaseCharacter::PawnSeenImpl"), *GetName());
    PawnSeen(SeenPawn);
}

void ABaseCharacter::PawnSeen(APawn *SeenPawn)
{
    //UE_LOG(LogTemp, Warning, TEXT("[%s]  ABaseCharacter::PawnSeen"), *GetName());
}
void ABaseCharacter::Attack()
{

}
bool ABaseCharacter::IsAlive()
{
    return (Attributes && Attributes->IsAlive());
}
bool ABaseCharacter::CanAttack()
{
    return false;
}


void ABaseCharacter::Die()
{
}

void ABaseCharacter::AttackEnd()
{
}
void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
    if(HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
    }
}
void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
    if(HitParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
    }
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
    if(Attributes)
    {
        Attributes->ReceivedDamage(DamageAmount);
    }
}

void ABaseCharacter::PlayHitReactMontage(const FName &SectionName)
{
    PlayMontageSection(HitReactMontage,SectionName);
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName & Section)
{
    UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
     if(AnimInstance && Montage) 
     {
        AnimInstance->Montage_Play(Montage);
        AnimInstance->Montage_JumpToSection(Section, Montage);
     }
}
void ABaseCharacter::DisableCapsule()
{
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
int32 ABaseCharacter::PlayAttackMontage()
{
    return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}
int32 ABaseCharacter::PlayDeathMontage()
{
    return PlayRandomMontageSection(DyingMontage, DyingMontageSections);
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
    int32 Selection = -1;
    if(SectionNames.Num() > 0)
    {
        Selection = FMath::RandRange(0,SectionNames.Num()-1);
        PlayMontageSection(Montage,SectionNames[Selection]);
    }
    return Selection;
}

void ABaseCharacter::StopAttackMontage()
{
        //stop any ongoing animation, if was playing attack montage, got hit, 
    UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance)
        AnimInstance->Montage_Stop(0.23, AttackMontage);
}

void ABaseCharacter::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
    StopAttackMontage();
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

    if(IsAlive() && Hitter)
        DirectionalHitReact(Hitter->GetActorLocation());
    else {
        Die();
    }

    PlayHitSound(ImpactPoint);
    SpawnHitParticles(ImpactPoint);
}
void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint) 
{
    const FVector Forward = GetActorForwardVector();
    //lower the impact point to the enemy actor location Z
    const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
    const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
    
    //Forward * ToHit == |Forward||ToHit|cos(theta)
    //|Forward|==1, |ToHit|==1 => Forward * ToHit == cos(theta)
    const double CosTheta = FVector::DotProduct(Forward, ToHit);
    
    double Theta = FMath::Acos(CosTheta);
    
    //from radiant to degrees
    Theta = FMath::RadiansToDegrees(Theta);
    
    //if crossproduct points down, theta should be negative
    const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
    if(CrossProduct.Z < 0)
        Theta *= 1.f;
    
    FName SectionName("FromBack");
    if(Theta >= -45.f && Theta < 45.f)
        SectionName = FName("FromFront");
    else if(Theta >= -135.f && Theta < -45.f)
        SectionName = FName("FromLeft");
    else if(Theta >= 45.f && Theta < 135.f)
        SectionName = FName("FromRight");
    
    PlayHitReactMontage(SectionName);
    
    /*UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation()+CrossProduct*100.f, 5.f, FColor::Blue, 5.f);
    
    if(GEngine) {
        GEngine->AddOnScreenDebugMessage(1, 60.0f, FColor::Cyan, FString::Printf(TEXT("Theta:%f"), Theta));
    }
    UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation()+Forward*60.f, 5.f, FColor::Red, 5.f);
    UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation()+ToHit*60.f, 5.f, FColor::Green, 5.f);*/
}
   
FVector ABaseCharacter::GetTranslationWarpTarget()
{
    if(CombatTarget)
    {
        const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
        const FVector MyLocation = GetActorLocation();
        FVector TargetToMe = (MyLocation - CombatTargetLocation).GetSafeNormal();
        TargetToMe *= WarpTargetDistance;

        //DRAW_SPHERE_COLOR(CombatTargetLocation + TargetToMe, FColor::Orange);

        return CombatTargetLocation + TargetToMe;
    }
    return FVector();
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
    if(CombatTarget)
    {
        return CombatTarget->GetActorLocation();
    }
    return FVector();
}

bool ABaseCharacter::InTargetRange(AActor *Target, double Radius)
{
    if (Target == nullptr) return false;
    const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
    return DistanceToTarget <= Radius;
}