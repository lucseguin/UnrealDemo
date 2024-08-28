// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"

#include "Enemy.generated.h"

UCLASS()
class LUCSLASHDEMO_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
    
    /** <AActor> */
    virtual void Tick(float DeltaTime) override;
    virtual void Destroyed() override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
 
    /** <IHitInterface> */
    virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
 protected:
    /** <AActor> */
	virtual void BeginPlay() override;
    
    /** <ABaseCharacter> */
    virtual void Die() override;
    virtual void Attack() override;
    virtual bool CanAttack() override;
    virtual void HandleDamage(float DamageAmount) override;
    virtual int32 PlayDeathMontage() override;
    virtual void AttackEnd() override;

    /**
     * AI Behaviours
     */
    void HideHealthBar();
    void ShowHealthBar();
    void LoseInterest();
    void StartPatrolling();
    void ChaseTarget();
    bool IsOutsideCombatRadius();
    bool IsOutsideAttackRadius();
    bool IsInsideAttackRadius();
    bool IsChasing();
    bool IsAttacking();
    bool IsEngaged();
    bool IsDead();
    virtual void PawnSeen(APawn* SeenPawn) override;

    UPROPERTY(BlueprintReadOnly)
    TEnumAsByte<EDeathPose> DeathPose;

    UPROPERTY(BlueprintReadOnly)
    EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
    /** AI Behaviour */
    void CheckCombatTarget();
    void CheckPatrolTarget();

    /**
     * Combat
     */
    void StartAttackTimer();
    void ClearAttackTimer();

    void ClearPatrolTimer();
    FTimerHandle PatrolTimer;

    UPROPERTY(EditAnywhere)
    TSubclassOf<class AWeapon> WeaponClass;
    
    UPROPERTY(VisibleAnywhere)
    class UHealthBarComponent* HealthBarWidget;

    UPROPERTY(EditAnywhere)
    double AttackRadius = 175.f;
    FTimerHandle AttackTimer;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackMin = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackMax = 1.0f;

    /** Patrolling Navigation*/
    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    AActor * PatrolTarget;

    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    TArray<AActor*> PatrolTargets;

    UPROPERTY(EditAnywhere)
    double PatrolRadius = 200.f;
    
    void PatrolTimerFinished();

    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float PatrolWaitMin = 5.f;

    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float PatrolWaitMax = 10.f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float PatrollingSpeed = 125.0f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float ChasingSpeed = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float DeathLifeSpan = 8.0f;

    void MoveToTarget(AActor* Target);
    AActor* ChoosePatrolTarget();

    class AAIController* EnemyController;
};
