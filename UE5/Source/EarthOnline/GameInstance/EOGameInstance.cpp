#include "GameInstance/EOGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

UEOGameInstance::UEOGameInstance()
{
    ServerAddress = TEXT("127.0.0.1");
    ServerPort = 7777;
    PlayerName = TEXT("玩家");
    bIsBetaMode = true;
}

void UEOGameInstance::Init()
{
    Super::Init();
    UE_LOG(LogTemp, Log, TEXT("地球Online GameInstance 初始化 - 内测模式: %s"), bIsBetaMode ? TEXT("是") : TEXT("否"));
}

void UEOGameInstance::JoinServer(const FString& Address, int32 Port)
{
    ServerAddress = Address;
    ServerPort = Port;

    if (GEngine)
    {
        FString URL = FString::Printf(TEXT("%s:%d"), *Address, Port);
        UE_LOG(LogTemp, Log, TEXT("正在连接服务器: %s"), *URL);
        GEngine->Browse(GetWorld(), *URL);
    }
}

void UEOGameInstance::HostServer(int32 Port, int32 MaxPlayers)
{
    ServerPort = Port;

    if (GEngine)
    {
        FString URL = FString::Printf(TEXT("/Game/Maps/TestMap?listen?port=%d?maxplayers=%d"), Port, MaxPlayers);
        UE_LOG(LogTemp, Log, TEXT("正在创建监听服务器: %s"), *URL);
        GEngine->Browse(GetWorld(), *URL);
    }
}

void UEOGameInstance::ReturnToMainMenu()
{
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));
}
