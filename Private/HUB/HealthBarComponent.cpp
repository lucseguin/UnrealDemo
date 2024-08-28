// Fill out your copyright notice in the Description page of Project Settings.


#include "HUB/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
    if(HealthBarWidget == nullptr)
        HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());

    if(HealthBarWidget && HealthBarWidget->HealthBar)
    {
        HealthBarWidget->HealthBar->SetPercent(Percent);

        if(Percent <= 0.25f) {
            FLinearColor CritialState(1.f, 0.f, 0.f);
            HealthBarWidget->HealthBar->SetFillColorAndOpacity(CritialState);
        }
    }
}