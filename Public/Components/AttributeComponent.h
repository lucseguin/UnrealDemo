// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUCSLASHDEMO_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
    //current active health
    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    float Health;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    float MaxHealth;
public:
    FORCEINLINE float GetHealth() const {return Health;}
    FORCEINLINE float GetMaxHealth() const {return MaxHealth;}
    FORCEINLINE float GetHealthPercent() const {return Health/MaxHealth;}
    void ReceivedDamage(float Damage);
    FORCEINLINE bool IsAlive() const { return (Health>0.f);}
};
