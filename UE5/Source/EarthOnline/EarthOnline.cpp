#include "EarthOnline.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FEarthOnlineModule, EarthOnline, "EarthOnline");

void FEarthOnlineModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("Immortal Dao Online模块已启动"));
}

void FEarthOnlineModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("Immortal Dao Online模块已关闭"));
}
