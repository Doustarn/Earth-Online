#include "UI/EOMainMenuWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "GameInstance/EOGameInstance.h"
#include "Kismet/GameplayStatics.h"

bool UEOMainMenuWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    if (HostButton) HostButton->OnClicked.AddDynamic(this, &UEOMainMenuWidget::OnHostClicked);
    if (JoinButton) JoinButton->OnClicked.AddDynamic(this, &UEOMainMenuWidget::OnJoinClicked);
    if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &UEOMainMenuWidget::OnQuitClicked);

    if (VersionText)
    {
        VersionText->SetText(FText::FromString(TEXT("地球Online v0.1.8 Beta 内测版")));
    }

    return true;
}

void UEOMainMenuWidget::OnHostClicked()
{
    if (UEOGameInstance* GI = Cast<UEOGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        int32 Port = 7777;
        if (PortInput)
        {
            Port = FCString::Atoi(*PortInput->GetText().ToString());
        }
        GI->HostServer(Port, 32);
    }
}

void UEOMainMenuWidget::OnJoinClicked()
{
    if (UEOGameInstance* GI = Cast<UEOGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        FString Address = TEXT("127.0.0.1");
        int32 Port = 7777;

        if (ServerAddressInput)
        {
            Address = ServerAddressInput->GetText().ToString();
            if (Address.IsEmpty()) Address = TEXT("127.0.0.1");
        }
        if (PortInput)
        {
            Port = FCString::Atoi(*PortInput->GetText().ToString());
            if (Port <= 0) Port = 7777;
        }

        GI->JoinServer(Address, Port);
    }
}

void UEOMainMenuWidget::OnQuitClicked()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->ConsoleCommand("quit");
    }
}
