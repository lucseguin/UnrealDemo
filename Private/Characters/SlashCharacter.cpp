// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"

#include "Components/InputComponent.h"

#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GroomComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimMontage.h"
#include "Item.h"
#include "Weapons/Weapon.h"

// Sets default values
ASlashCharacter::ASlashCharacter() : ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
    
    GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic,  ECollisionResponse::ECR_Overlap);
    GetMesh()->SetGenerateOverlapEvents(true);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->TargetArmLength = 300.f;
    
    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(SpringArm);
    
    Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
    Hair->SetupAttachment(GetMesh());
    Hair->AttachmentName = FString("head");
    
    Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
    Eyebrows->SetupAttachment(GetMesh());
    Eyebrows->AttachmentName = FString("head");
    
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    if(PawnSensingCmp)
        PawnSensingCmp->bOnlySensePlayers = false;

    if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(SlashContext, 0);
        }
    }

    Tags.Add(FName("EngageableTarget"));
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this,  &ASlashCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,  &ASlashCharacter::Look);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this,  &ACharacter::Jump);
        EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this,  &ASlashCharacter::EKeyPressed);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this,  &ASlashCharacter::Attack);
        EnhancedInputComponent->BindAction(FarAttackAction, ETriggerEvent::Triggered, this,  &ASlashCharacter::FarAttack);
        EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this,  &ASlashCharacter::Dodge);
    }
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
    if(ActionState == EActionState::EAS_Unoccupied)
    {
        const FVector2D Movement = Value.Get<FVector2D>();
        if(Controller && (Movement.X != 0 || Movement.Y!=0)) {
            const FRotator Rotation = Controller->GetControlRotation();
            const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
            
            const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
            AddMovementInput(ForwardDirection, Movement.Y);
            
            const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
            AddMovementInput(RightDirection, Movement.X);
        }
    }
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisValue = Value.Get<FVector2D>();
    if(Controller && (LookAxisValue.X != 0 || LookAxisValue.Y!=0)) {
        AddControllerYawInput(LookAxisValue.X /** GetWorld()->GetDeltaSeconds()*/);
        AddControllerPitchInput(LookAxisValue.Y /** GetWorld()->GetDeltaSeconds()*/);
    }
}

void ASlashCharacter::EKeyPressed()
{
    AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
    if(OverlappingWeapon)
    {
        if(EquippedWeapon) {
            EquippedWeapon->Unequip();
            EquippedWeapon = nullptr;
        }
        if(OverlappingWeapon->WeaponType == EWeaponType::EWT_OneHandMelee)
        {
            OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
            CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
        }
        else if(OverlappingWeapon->WeaponType == EWeaponType::EWT_TwoHandMelee)
        {
            OverlappingWeapon->Equip(GetMesh(), FName("RightHand2Weapon"), this, this);
            CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
        }
        
        OverlappingItem = nullptr;
        EquippedWeapon = OverlappingWeapon;
    }
    else
    {
    
        if(CanDisarm())
        {
            PlayMontageEquip(FName("Unequip"));
            CharacterState = ECharacterState::ECS_Unequipped;
            ActionState = EActionState::EAS_EquippingWeapon;
        }
        else if(CanArm())
        {
            PlayMontageEquip(FName("Equip"));

            if(EquippedWeapon->WeaponType == EWeaponType::EWT_OneHandMelee)
            {
                CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
            }
            else if(EquippedWeapon->WeaponType == EWeaponType::EWT_TwoHandMelee)
            {
                CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
            }
            ActionState = EActionState::EAS_EquippingWeapon;
        }
    }
            
}
bool ASlashCharacter::CanAttack()
{
    return ActionState == EActionState::EAS_Unoccupied &&
            CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanDisarm()
{
    return (EquippedWeapon && CharacterState != ECharacterState::ECS_Unequipped && ActionState == EActionState::EAS_Unoccupied);
}
bool ASlashCharacter::CanArm()
{
    return (EquippedWeapon && CharacterState == ECharacterState::ECS_Unequipped && ActionState == EActionState::EAS_Unoccupied);
}
void ASlashCharacter::Attack()
{
    Super::Attack();

    if(CanAttack())
    {
        PlayAttackMontage();
        ActionState = EActionState::EAS_Attacking;
    }
}

void ASlashCharacter::FarAttack()
{
    Super::Attack();

    if(CanAttack())
    {
        PlayFarAttackMontage();
        ActionState = EActionState::EAS_Attacking;
    }
}

void ASlashCharacter::Disarm()
{
    if(EquippedWeapon)
    {
        if(EquippedWeapon->WeaponType == EWeaponType::EWT_OneHandMelee)
        {
            EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
        }
        else if(EquippedWeapon->WeaponType == EWeaponType::EWT_TwoHandMelee)
        {
            EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("TwoHandSpineSocket"));
        }             
    }
}

void ASlashCharacter::Arm()
{
    if(EquippedWeapon)
    {
        if(EquippedWeapon->WeaponType == EWeaponType::EWT_OneHandMelee)
        {
            EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
        }
        else if(EquippedWeapon->WeaponType == EWeaponType::EWT_TwoHandMelee)
        {
            EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHand2Weapon"));
        }        
    }
}
void ASlashCharacter::FinishEquipping()
{
    ActionState = EActionState::EAS_Unoccupied;
}

int32 ASlashCharacter::PlayAttackMontage()
{
    if(EquippedWeapon->WeaponType == EWeaponType::EWT_OneHandMelee)
    {
        return Super::PlayAttackMontage();
    }
    else if(EquippedWeapon->WeaponType == EWeaponType::EWT_TwoHandMelee)
    {
        return PlayTwoHandAttack();
    }       
    return -1;
}

int32 ASlashCharacter::PlayFarAttackMontage()
{
    if(EquippedWeapon->WeaponType == EWeaponType::EWT_OneHandMelee)
    {
        return PlayRandomMontageSection(FarAttackMontage, FarAttackMontageSections);
    }
  
    return -1;
}


int32 ASlashCharacter::PlayTwoHandAttack()
{
     
    UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && TwoHandAttackMontage)
    {
        AnimInstance->Montage_Play(TwoHandAttackMontage);
        const int32 Selection = FMath::RandRange(1,2);
        FName SectionName = FName(FString::Printf(TEXT("Attack%d"), Selection));
        AnimInstance->Montage_JumpToSection(SectionName, TwoHandAttackMontage);
        return Selection;
    }
    else
    {
        return -1;
    }
}
void ASlashCharacter::PlayMontageEquip(const FName& SectionName)
{
        if(EquippedWeapon->WeaponType == EWeaponType::EWT_OneHandMelee)
        {
            PlaySingleHandMontageEquip(SectionName);
        }
        else if(EquippedWeapon->WeaponType == EWeaponType::EWT_TwoHandMelee)
        {
            PlayTwoHandMontageEquip(SectionName);
        }        
}

void ASlashCharacter::PlaySingleHandMontageEquip(const FName& SectionName)
{
    UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && EquipMontage)
    {
        AnimInstance->Montage_Play(EquipMontage);
        AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
    }
}
void ASlashCharacter::PlayTwoHandMontageEquip(const FName& SectionName)
{
    UAnimInstance * AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && TwoHandEquipMontage)
    {
        AnimInstance->Montage_Play(TwoHandEquipMontage);
        AnimInstance->Montage_JumpToSection(SectionName, TwoHandEquipMontage);
    }
}

void ASlashCharacter::AttackEnd()
{
    ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Dodge()
{

}

void ASlashCharacter::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
    Super::GetHit_Implementation(ImpactPoint,Hitter);
    ActionState = EActionState::EAS_HitReaction;
    UE_LOG(LogTemp, Warning, TEXT("[%s] Character has been hit"), *GetName());
}

void ASlashCharacter::HitReactEnd()
{
    UE_LOG(LogTemp, Warning, TEXT("[%s] HitReactEnd"), *GetName());
    ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::PawnSeen(APawn *SeenPawn)
{
    Super::PawnSeen(SeenPawn);
    //UE_LOG(LogTemp, Warning, TEXT("[%s] Character has seen Pawn"), *GetName());

    if(SeenPawn->ActorHasTag(FName("Enemy")) &&  InTargetRange(SeenPawn, CombatRadius) && CombatTarget !=SeenPawn){
        if(CombatTarget) 
            OutlineActor(CombatTarget, false);

        CombatTarget = SeenPawn;
        OutlineActor(CombatTarget, true);
        UE_LOG(LogTemp, Warning, TEXT("[%s] Outline Actor"), *GetName());
    }
}

void ASlashCharacter::OutlineActor(AActor *Actor, bool bOutline)
{
    TArray<UStaticMeshComponent*> Components;
	Actor->GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++)
	{
		UStaticMeshComponent* StaticMeshComponent = Components[i];
        StaticMeshComponent->SetRenderCustomDepth(bOutline); 
	}

    TArray<USkeletalMeshComponent*> SkeletalComponents;
	Actor->GetComponents<USkeletalMeshComponent>(SkeletalComponents);
	for (int32 i = 0; i < SkeletalComponents.Num(); i++)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = SkeletalComponents[i];
        SkeletalMeshComponent->SetRenderCustomDepth(bOutline); 
	}
    
    // UActorComponent* comp = Actor->GetComponentByClass(UStaticMeshComponent::StaticClass());
    // UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(comp);
    // StaticMeshComp->SetRenderCustomDepth(bOutline);
}
//void ASlashCharacter::Jump()
//{
//    if(!GetCharacterMovement()->IsFalling()) {
//        Super::Jump();
//    }
//}

