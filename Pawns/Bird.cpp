// Fill out your copyright notice in the Description page of Project Settings.


#include "Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABird::ABird()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    Capsule->SetCapsuleHalfHeight(20.f);
    Capsule->SetCapsuleRadius(15.f);
    
    SetRootComponent(Capsule);
    
    BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
    BirdMesh->SetupAttachment(GetRootComponent());
    
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->TargetArmLength = 300.f;
    
    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(SpringArm);
    
    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABird::BeginPlay()
{
	Super::BeginPlay();
	
    if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(BirdMappingContext, 0);
        }
    }
}

// Called every frame
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABird::Move(const FInputActionValue& Value)
{
    const float DirectionValue = Value.Get<float>();
    UE_LOG(LogTemp, Warning, TEXT("ABird::Move Triggered DirectionValue:%f"), DirectionValue);
    
    if(Controller && (DirectionValue != 0.f))
    {
        FVector Forward = GetActorForwardVector();
        AddMovementInput(Forward, DirectionValue);
    }
}

void ABird::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisValue = Value.Get<FVector2D>();

    if(GetController())
    {
        UE_LOG(LogTemp, Warning, TEXT("ABird::Look Triggered X:%f Y:%f"), LookAxisValue.X, LookAxisValue.Y);
        AddControllerYawInput(LookAxisValue.X * GetWorld()->GetDeltaSeconds());
        AddControllerPitchInput(LookAxisValue.Y * GetWorld()->GetDeltaSeconds());
    }
}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
        
    //PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);
    if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,  &ABird::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,  &ABird::Look);
    }
}

