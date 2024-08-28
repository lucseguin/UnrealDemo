//
//  CharacterTypes.h
//  LucSlashDemo_Build
//
//  Created by Luc Seguin on 2023-12-14.
//  Copyright © 2023 Epic Games, Inc. All rights reserved.
//

#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    ECS_Unequipped UMETA(DisplayName = "Unequipped"),
    ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
    ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
    EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
    EAS_HitReaction UMETA(DisplayName = "HitReaction"),
    EAS_Attacking UMETA(DisplayName = "Attacking"),
    EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon")
};

UENUM(BlueprintType)
enum EDeathPose
{
    EDP_Death1 UMETA(DisplayName = "Death1"),
    EDP_Death2 UMETA(DisplayName = "Death2"),
    EDP_Death3 UMETA(DisplayName = "Death3"),
    EDP_Death4 UMETA(DisplayName = "Death4"),
    EDP_Death5 UMETA(DisplayName = "Death5"), 

    EDP_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    EWT_OneHandMelee UMETA(DisplayName = "One Hand Melee"),
    EWT_TwoHandMelee UMETA(DisplayName = "Two Hand Melee"),
    EWT_OneHandGun UMETA(DisplayName = "One Hand Gun"),
    EWT_TwoHandGun UMETA(DisplayName = "Two Hand Gun")

};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    EES_Dead UMETA(DisplayName = "Dead"),
    EES_Patrolling UMETA(DisplayName = "Patrolling"),
    EES_Chasing UMETA(DisplayName = "Chasing"),
    EES_Attacking UMETA(DisplayName = "Attacking"),
    EES_Engaged UMETA(DisplayName = "Engaged"),

    EES_NoState UMETA(DisplayName = "NoState")
};
