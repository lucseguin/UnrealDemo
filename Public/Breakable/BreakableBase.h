// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableBase.generated.h"

UCLASS()
class LUCSLASHDEMO_API ABreakableBase : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	

    virtual void Tick(float DeltaTime) override;
    virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
    
protected:
    ABreakableBase();
    
	virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UGeometryCollectionComponent* GeometryCollection;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UShapeComponent* BlockingShape;
    
    UFUNCTION()
    void OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent);
    
private:
    UPROPERTY(EditAnywhere, Category = "Breakable Properties")
    TArray<TSubclassOf<class ATreasure>> TreasureClasses;
    
    UPROPERTY(EditAnywhere, Category = Sounds)
    USoundBase* BreakingSound;
    
    bool bBroken = false;
    
    void ProcessHit(const FVector& ImpactPoint);

};
