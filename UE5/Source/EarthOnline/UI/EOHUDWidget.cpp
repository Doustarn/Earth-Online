#include "UI/EOHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Character/EOBaseCharacter.h"
#include "Kismet/GameplayStatics.h"

bool UEOHUDWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    NotificationDuration = 5.0f;
    return true;
}

void UEOHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (AEOBaseCharacter* Player = Cast<AEOBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
        if (PlayerNameText)
        {
            PlayerNameText->SetText(FText::FromString(Player->PlayerName));
        }

        if (HealthBar)
        {
            HealthBar->SetPercent(Player->Health / Player->MaxHealth);
        }

        if (StaminaBar)
        {
            StaminaBar->SetPercent(Player->Stamina / Player->MaxStamina);
        }

        if (PositionText)
        {
            FVector Loc = Player->GetActorLocation();
            PositionText->SetText(FText::FromString(
                FString::Printf(TEXT("X: %.0f  Y: %.0f  Z: %.0f"), Loc.X, Loc.Y, Loc.Z)
            ));
        }
    }
}

void UEOHUDWidget::ShowNotification(const FString& Message)
{
    if (NotificationList)
    {
        UTextBlock* NewNote = NewObject<UTextBlock>(this);
        if (NewNote)
        {
            NewNote->SetText(FText::FromString(Message));
            NewNote->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
            NotificationList->AddChildToVerticalBox(NewNote);

            PendingNotifications.Add(Message);
        }
    }
}
