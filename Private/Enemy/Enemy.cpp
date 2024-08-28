// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "HUB/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Weapons/Weapon.h"
#include "DebugMacros.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemy::AEnemy() : ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetMesh()->SetGenerateOverlapEvents(true);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    
    HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
    HealthBarWidget->SetupAttachment(GetRootComponent());

    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

    HideHealthBar();

    if(Attributes && HealthBarWidget)
        HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());   

    EnemyController = Cast<AAIController>(GetController());
    MoveToTarget(PatrolTarget);

    Tags.Add(FName("Enemy"));

    UWorld* World = GetWorld();
    if(World && WeaponClass) 
    {
        AWeapon* DefaultWeapon =  World->SpawnActor<AWeapon>(WeaponClass);
        DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
        EquippedWeapon = DefaultWeapon; 
    }
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if(IsDead()) return;

    //DRAW_SPHERE_COLOR_RADIUS_SingleFrame(GetActorLocation(), CombatRadius, FColor::Blue);
    //DRAW_SPHERE_COLOR_RADIUS_SingleFrame(GetActorLocation(), AttackRadius, FColor::Red);

    if(EnemyState > EEnemyState::EES_Patrolling )
        CheckCombatTarget();    
    else
        CheckPatrolTarget();
}

int32 AEnemy::PlayDeathMontage()
{
    const int32 Selection = Super::PlayDeathMontage();
    if(Selection>=0)
    {
        TEnumAsByte<EDeathPose> Pose(Selection);
        if(Pose < EDeathPose::EDP_MAX)
            DeathPose = Pose;
    }
    return Selection;
}
void AEnemy::Die()
{
    EnemyState = EEnemyState::EES_Dead;
    ClearAttackTimer();
    HideHealthBar();
    PlayDeathMontage();
    DisableCapsule();
    SetLifeSpan(DeathLifeSpan);
    GetCharacterMovement()->bOrientRotationToMovement = false;
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
    //UE_LOG(LogTemp, Warning, TEXT("[%s] Attacking"),*GetName());
    EnemyState = EEnemyState::EES_Engaged;
    Super::Attack();
    PlayAttackMontage();
}

void AEnemy::MoveToTarget(AActor *Target)
{
    if(EnemyController && Target){
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalActor(Target);
        MoveRequest.SetAcceptanceRadius(75.f);
        EnemyController->MoveTo(MoveRequest);
    }
}
AActor *AEnemy::ChoosePatrolTarget()
{
    TArray<AActor*> ValidTargets;
    for(AActor* Target : PatrolTargets) {
        if(Target != PatrolTarget) {
            ValidTargets.AddUnique(Target);
        }
    }

    if(ValidTargets.Num() > 0) {
        const int32 Selection = FMath::RandRange(0,ValidTargets.Num()-1);
        return ValidTargets[Selection];
     } 

    return nullptr;
}
void AEnemy::HideHealthBar() 
{
    if(HealthBarWidget) 
        HealthBarWidget->SetVisibility(false);
}

void AEnemy::ShowHealthBar() 
{
    if(HealthBarWidget) 
        HealthBarWidget->SetVisibility(true);
}

void AEnemy::LoseInterest() {
    //UE_LOG(LogTemp, Warning, TEXT("[%s] Lost Interest"),*GetName());
    CombatTarget = nullptr; 
    HideHealthBar();
}

void AEnemy::StartPatrolling()
{
    //UE_LOG(LogTemp, Warning, TEXT("[%s] Start Patrolling"),*GetName());
    EnemyState = EEnemyState::EES_Patrolling;
    GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
    MoveToTarget(PatrolTarget);
}
void AEnemy::ChaseTarget()
{
    //UE_LOG(LogTemp, Warning, TEXT("[%s] Chasing"),*GetName());
    EnemyState = EEnemyState::EES_Chasing;
    GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
    MoveToTarget(CombatTarget);
    ShowHealthBar();
}
bool AEnemy::IsOutsideCombatRadius()
{
    return !InTargetRange(CombatTarget, CombatRadius);
}
bool AEnemy::IsOutsideAttackRadius()
{
    return !InTargetRange(CombatTarget, AttackRadius);
}
bool AEnemy::IsInsideAttackRadius()
{
    return InTargetRange(CombatTarget, AttackRadius);
}
bool AEnemy::IsChasing()
{
    return EnemyState == EEnemyState::EES_Chasing;
}
bool AEnemy::IsAttacking()
{
    return EnemyState == EEnemyState::EES_Attacking;
}
bool AEnemy::IsDead()
{
    return EnemyState == EEnemyState::EES_Dead;
}
bool AEnemy::IsEngaged()
{
    return EnemyState == EEnemyState::EES_Engaged;
}
void AEnemy::StartAttackTimer()
{
    //UE_LOG(LogTemp, Warning, TEXT("[%s] Will Attack"),*GetName());
    EnemyState = EEnemyState::EES_Attacking;
    const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
    GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
    GetWorldTimerManager().ClearTimer(AttackTimer);
}
void AEnemy::AttackEnd()
{
    EnemyState = EEnemyState::EES_NoState;
    CheckCombatTarget();
}
bool AEnemy::CanAttack()
{
    bool bCanAttack = IsInsideAttackRadius() && 
                     !IsAttacking() &&
                     !IsDead() &&
                     !IsEngaged();
    return bCanAttack;
}
void AEnemy::CheckCombatTarget()
{
    if(IsOutsideCombatRadius()) 
    {
        ClearAttackTimer();
        LoseInterest();
        if(!IsEngaged())
            StartPatrolling();
    } 
    else if(IsOutsideAttackRadius() && !IsChasing()) 
    {
        ClearAttackTimer();
        if(!IsEngaged())
            ChaseTarget();
    }
    else if(CanAttack())
    {
        StartAttackTimer();
    }
}
void AEnemy::CheckPatrolTarget()
{
    if(InTargetRange(PatrolTarget, PatrolRadius)) 
    {
        PatrolTarget = ChoosePatrolTarget();
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, FMath::RandRange(PatrolWaitMin,PatrolWaitMax));
    }    
}

void AEnemy::ClearPatrolTimer()
{
    GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::PawnSeen(APawn *SeenPawn)
{
    Super::PawnSeen(SeenPawn);
    //UE_LOG(LogTemp, Warning, TEXT("[%s] Pawn Seen - Is Character %s"), *GetName(), (SeenPawn->ActorHasTag(FName("SlashCharacter"))?TEXT("true"):TEXT("false")) );
    
    const bool bShouldChaseTarget = 
        EnemyState != EEnemyState::EES_Dead && 
        EnemyState != EEnemyState::EES_Chasing &&
        EnemyState < EEnemyState::EES_Chasing &&
        SeenPawn->ActorHasTag(FName("EngageableTarget")) &&
        InTargetRange(SeenPawn, CombatRadius);
   
    if(bShouldChaseTarget) 
    {
        //UE_LOG(LogTemp, Warning, TEXT("[%s] Character Seen in Combat Radius"), *GetName());
        CombatTarget = SeenPawn;
        ClearPatrolTimer();
        ChaseTarget();
    }
}
void AEnemy::PatrolTimerFinished()
{
     MoveToTarget(PatrolTarget);
}
void AEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
    Super::GetHit_Implementation(ImpactPoint, Hitter);
    if(!IsDead())ShowHealthBar();
    ClearPatrolTimer();
    ClearAttackTimer(); 

	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}    
}
void AEnemy::HandleDamage(float DamageAmount)
{
    Super::HandleDamage(DamageAmount);
    if(HealthBarWidget)
        HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());        
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
    HandleDamage(DamageAmount);

    CombatTarget = EventInstigator->GetPawn();
    
    if(IsInsideAttackRadius())
        EnemyState = EEnemyState::EES_Attacking;
    else if(IsOutsideAttackRadius())
        ChaseTarget();
    
    return DamageAmount;
}

void AEnemy::Destroyed()
{
        Super::Destroyed();
        if(EquippedWeapon) {
            EquippedWeapon->Destroy();
        }
}
