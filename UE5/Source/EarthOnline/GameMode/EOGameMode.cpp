#include "GameMode/EOGameMode.h"
#include "PlayerController/EOPlayerController.h"
#include "Character/EOBaseCharacter.h"
#include "HUD/EOHUD.h"

AEOGameMode::AEOGameMode()
{
    MaxPlayers = 32;
    GameName = TEXT("地球Online");
    BetaVersion = TEXT("0.1.8 Beta");

    PlayerControllerClass = AEOPlayerController::StaticClass();
    DefaultPawnClass = AEOBaseCharacter::StaticClass();
    HUDClass = AEOHUD::StaticClass();

    bUseSeamlessTravel = true;
}

void AEOGameMode::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("=== 地球Online 内测服启动 ==="));
    UE_LOG(LogTemp, Log, TEXT("版本: %s"), *BetaVersion);
    UE_LOG(LogTemp, Log, TEXT("最大玩家数: %d"), MaxPlayers);
}

void AEOGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (AEOPlayerController* PC = Cast<AEOPlayerController>(NewPlayer))
    {
        PC->Client_ShowWelcomeMessage(BetaVersion);
        UE_LOG(LogTemp, Log, TEXT("玩家加入服务器，当前人数: %d"), GetCurrentPlayerCount());
    }
}

void AEOGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    UE_LOG(LogTemp, Log, TEXT("玩家离开服务器，当前人数: %d"), GetCurrentPlayerCount());
}

int32 AEOGameMode::GetCurrentPlayerCount() const
{
    return GetNumPlayers();
}
