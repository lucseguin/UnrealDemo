// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class UGroomComponent;

UCLASS()
class LUCSLASHDEMO_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
    
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
    FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
    
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    void OutlineActor(AActor *Actor, bool bOutline);
    virtual void GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter) override;

    UPROPERTY(EditAnywhere, Category = Input)
    UInputMappingContext* SlashContext;
    
    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* MovementAction;
    
    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* LookAction;
    
    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* JumpAction;
    
    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* EKeyAction;
    
    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* AttackAction;

    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* FarAttackAction;
    
    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* DodgeAction;
    
    /**
     Callbacks for input
     */
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void EKeyPressed();
    virtual void Attack() override;
    void Dodge();
    void FarAttack();
    
    /**
     Play montage attacks
     */
    virtual int32 PlayAttackMontage() override;
    int32 PlayTwoHandAttack();
    int32 PlayFarAttackMontage();

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UAnimMontage* FarAttackMontage;
    UPROPERTY(EditAnywhere, Category = "Combat")
    TArray<FName> FarAttackMontageSections;

    void PlayMontageEquip(const FName& SectionName);
    void PlaySingleHandMontageEquip(const FName& SectionName);
    void PlayTwoHandMontageEquip(const FName& SectionName);

    virtual bool CanAttack() override;
    bool CanDisarm();
    bool CanArm();

    virtual void PawnSeen(APawn* SeenPawn) override;

    /**
     Blueprint Anim notify related events
     */
    virtual void AttackEnd() override;
    
    UFUNCTION(BlueprintCallable)
    void Disarm();
    
    UFUNCTION(BlueprintCallable)
    void Arm();
    
    UFUNCTION(BlueprintCallable)
    void FinishEquipping();
    
    UFUNCTION(BlueprintCallable)
    void HitReactEnd();
private:
    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;
    
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* ViewCamera;

    UPROPERTY(VisibleInstanceOnly)
    AItem * OverlappingItem;

    UPROPERTY(VisibleInstanceOnly)
    ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
    
    UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess= "true"))
    EActionState ActionState = EActionState::EAS_Unoccupied;
    
    /**
     * Animation Montages
     */
    UPROPERTY(EditDefaultsOnly, Category = Montages)
    UAnimMontage* EquipMontage;
      
    UPROPERTY(EditDefaultsOnly, Category = Montages)
    UAnimMontage* TwoHandAttackMontage;
    
    UPROPERTY(EditDefaultsOnly, Category = Montages)
    UAnimMontage* TwoHandEquipMontage;

    UPROPERTY(VisibleAnywhere, Category = Hair)
    UGroomComponent* Hair;
    
    UPROPERTY(VisibleAnywhere, Category = Hair)
    UGroomComponent* Eyebrows;


};
