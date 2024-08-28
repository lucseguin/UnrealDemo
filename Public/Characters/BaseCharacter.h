// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;
class UPawnSensingComponent;

UCLASS()
class LUCSLASHDEMO_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
    void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    virtual void GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter) override;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
    AWeapon * EquippedWeapon;

    UPROPERTY(VisibleAnywhere)
    class UAttributeComponent* Attributes;

    /**
     * Animation Montages
     */
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* AttackMontage;
    UPROPERTY(EditAnywhere, Category = "Combat")
    TArray<FName> AttackMontageSections;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* HitReactMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* DyingMontage;
    UPROPERTY(EditAnywhere, Category = "Combat")
    TArray<FName> DyingMontageSections;

	UPROPERTY(EditAnywhere, Category = "Combat")
    USoundBase* HitSound;

    UPROPERTY(EditAnywhere, Category = "Combat")
    UParticleSystem* HitParticles;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    AActor* CombatTarget;
    
    UPROPERTY(EditAnywhere, Category = "Combat")
    double WarpTargetDistance = 75.f;

    UPROPERTY(VisibleAnywhere)
    UPawnSensingComponent* PawnSensingCmp;

    UPROPERTY(EditAnywhere)
    double CombatRadius = 750.f;

    UPROPERTY(EditAnywhere)
    double PeripheralVisonAngle = 45.f;

    bool InTargetRange(AActor* Target, double Radius);

    virtual void PawnSeen(APawn* SeenPawn); 
    /**
     Play montage attacks
     */
	virtual int32 PlayAttackMontage();
    virtual int32 PlayDeathMontage();
    virtual void PlayHitReactMontage(const FName& SectionName);
    void PlayMontageSection(UAnimMontage* Montage, const FName & Section);
    int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
    void DisableCapsule();
    void StopAttackMontage();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    FVector GetTranslationWarpTarget();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    FVector GetRotationWarpTarget();

    void PlayHitSound(const FVector& ImpactPoint);
    void SpawnHitParticles(const FVector& ImpactPoint);
    virtual void HandleDamage(float DamageAmount);


	virtual void Attack();
	virtual bool CanAttack();
    virtual bool IsAlive();
 	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void Die();
	
    UFUNCTION(BlueprintCallable)
    virtual void AttackEnd();

private:
    UFUNCTION()
    void PawnSeenImpl(APawn* SeenPawn);  // CallBack for OnPawmSeen in UPawnSensingComponent
};
