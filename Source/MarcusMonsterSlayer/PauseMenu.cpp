// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu.h"
#include "Marcus.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UPauseMenu::NativeConstruct()
{
    Super::NativeConstruct();

    if (ResumeButton)
    {
        ResumeButton->OnClicked.AddDynamic(this, &UPauseMenu::OnResumeClicked);
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UPauseMenu::OnQuitClicked);
    }
}

void UPauseMenu::OnResumeClicked()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    AMarcus* Marcus = Cast<AMarcus>(PlayerPawn);

    if (!Marcus) return;

    Marcus->UnPauseGame();

}

void UPauseMenu::OnQuitClicked()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    
    if (!PlayerController)
    {
        return;
    }

    UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
  
}