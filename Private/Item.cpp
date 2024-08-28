// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Characters/SlashCharacter.h"
#include "NiagaraComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
    RootComponent = ItemMesh;
    ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    Sphere->SetupAttachment(GetRootComponent());
    
    EmbersEffect= CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
    EmbersEffect->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
    Super::BeginPlay();
    
    //UE_LOG(LogTemp, Warning, TEXT("Item BeginPlay Called"));
    
//    if(GEngine) {
//        GEngine->AddOnScreenDebugMessage(1, 60.0f, FColor::Cyan, FString("Item OnScreen Message"));
//    }
    
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
    Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
    
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    RunningTime += DeltaTime;
    
    if(ItemState == EItemState::EIS_Hovering)
    {
        AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
    }
    
    //E_LOG(LogTemp, Warning, TEXT("Delta Time : %f"), DeltaTime);
//    if(GEngine) {
//        FString Name = GetName();
//        FString Message = FString::Printf(TEXT("[%s] Delta Time : %f"), *Name, DeltaTime);
//        GEngine->AddOnScreenDebugMessage(1, 60.0f, FColor::Cyan, Message);
//    }
    
//    float MovementRate = 50.f;
//    float RotationRate = 45.f;
//    
//    AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));
//    AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
    
//    UWorld * World = GetWorld();
//    if(World) {
//        FVector Location = GetActorLocation();
//        
//        //DrawDebugLine(World, Location, GetActorForwardVector()*100.0f+Location, FColor::Green, true);
//        //DrawDebugPoint(World, GetActorForwardVector()*100.0f+Location, 15.0f, FColor::Green, true);
//        DrawDebugDirectionalArrow(World, Location, GetActorForwardVector()*100.0f+Location, 15.0f, FColor::Green, false, -1.f);
//        DrawDebugSphere(World, Location, 50.0f, 24, FColor::Cyan, false, -1.f);
//    }
}

float AItem::TransformedSin()
{
    return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}
float AItem::TransformedCos()
{
    return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}
template<typename T>
T AItem::Avg(T First, T Second) 
{
    return (First + Second) / 2;
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
//    const FString OtherActorNAme = OtherActor->GetName();
//    if(GEngine) {
//        FString Message = FString::Printf(TEXT("OnSphereOverlap with OtherActor[%s]"), *OtherActorNAme);
//        GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Red, Message);
//    }
    
    //this behaviour assumes only a single item will overlap at a time.
    ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
    if(SlashCharacter)
    {
        SlashCharacter->SetOverlappingItem(this);
    }
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
//    const FString OtherActorNAme = OtherActor->GetName();
//    if(GEngine) {
//        FString Message = FString::Printf(TEXT("OnSphereEndOverlap with OtherActor[%s]"), *OtherActorNAme);
//        GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Green, Message);
//    }
    
    //this behaviour assumes only a single item will overlap at a time. 
    
    ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
    if(SlashCharacter)
    {
        SlashCharacter->SetOverlappingItem(nullptr);
    }
}
