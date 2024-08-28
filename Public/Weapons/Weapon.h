// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Characters/CharacterTypes.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class LUCSLASHDEMO_API AWeapon : public AItem
{
    GENERATED_BODY()
public:
    AWeapon();
    void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
    void Unequip();
    void AttachMeshToSocket(USceneComponent *InParent, FName InSocketName);
    
    FORCEINLINE UBoxComponent* GetWeaponBox() const {return WeaponBox; }
    
    TArray<AActor*> IgnoreActors;

    UPROPERTY(EditAnywhere,Category = "Weapon Property")
    EWeaponType WeaponType;
    
protected:
    virtual void BeginPlay() override;
    
    UFUNCTION()
    void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

    UFUNCTION(BlueprintImplementableEvent)
    void CreateFields(const FVector& FiledLocation);

    void PlayEquipSound();
    void DisableSphereCollision();
    void DeactivateEmbers();
    void ExecuteGetHit(const FHitResult& BoxHit);
    bool IsSameActorType(AActor* OtherActor);
private:
    UPROPERTY(EditAnywhere, Category = "Weapon Property")
    USoundBase* EquipSound;

    UPROPERTY(VisibleAnywhere, Category = "Weapon Property")
    UBoxComponent* WeaponBox;
    
    UPROPERTY(VisibleAnywhere, Category = "Weapon Property")
    USceneComponent* BoxTraceStart;
    
    UPROPERTY(VisibleAnywhere, Category = "Weapon Property")
    USceneComponent* BoxTraceEnd;
    
    UPROPERTY(EditAnywhere,Category = "Weapon Property")
    float Damage = 20.f;

    void BoxTrace(FHitResult& BoxHit);

    UPROPERTY(EditAnywhere, Category = "Weapon Property")
    FVector BoxTraceExtent = FVector(5.f, 5.f, 5.f);

    UPROPERTY(EditAnywhere, Category = "Weapon Property")   
    bool bShowBoxDebug = false;

};
