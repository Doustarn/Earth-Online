// Immortal Dao Online - UGC玩家自建内容管理器实现
#include "EOUGCManager.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/DateTime.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"

void UEOUGCManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    EnsureDirExists(GetUGCRootDir());
    EnsureDirExists(GetPlayerMapsDir());
    EnsureDirExists(GetPlayerModelsDir());
    UE_LOG(LogTemp, Log, TEXT("[UGC] 玩家自建内容系统已初始化"));
}

void UEOUGCManager::Deinitialize()
{
    Super::Deinitialize();
}

FString UEOUGCManager::GetUGCRootDir() const
{
    return FPaths::ProjectSavedDir() / TEXT("UGC");
}

FString UEOUGCManager::GetPlayerMapsDir() const
{
    return GetUGCRootDir() / TEXT("Maps");
}

FString UEOUGCManager::GetPlayerModelsDir() const
{
    return GetUGCRootDir() / TEXT("Models");
}

bool UEOUGCManager::SaveMap(const FUGCMapData& MapData, const FString& MapName)
{
    FString SafeName = SanitizeFileName(MapName);
    if (SafeName.IsEmpty()) return false;

    FString FilePath = GetPlayerMapsDir() / SafeName + TEXT(".eomap");

    // JSON序列化
    TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject());
    RootObj->SetStringField(TEXT("MapName"), MapData.MapName);
    RootObj->SetStringField(TEXT("Author"), MapData.Author);
    RootObj->SetStringField(TEXT("Description"), MapData.Description);
    RootObj->SetStringField(TEXT("CreatedTime"), MapData.CreatedTime.ToString());
    RootObj->SetStringField(TEXT("ModifiedTime"), MapData.ModifiedTime.ToString());

    TArray<TSharedPtr<FJsonValue>> ObjectsArray;
    for (const FUGCObjectData& Obj : MapData.Objects)
    {
        TSharedPtr<FJsonObject> ObjJson = MakeShareable(new FJsonObject());
        ObjJson->SetStringField(TEXT("Class"), Obj.ObjectClassPath);
        ObjJson->SetNumberField(TEXT("LocX"), Obj.Location.X);
        ObjJson->SetNumberField(TEXT("LocY"), Obj.Location.Y);
        ObjJson->SetNumberField(TEXT("LocZ"), Obj.Location.Z);
        ObjJson->SetNumberField(TEXT("RotP"), Obj.Rotation.Pitch);
        ObjJson->SetNumberField(TEXT("RotY"), Obj.Rotation.Yaw);
        ObjJson->SetNumberField(TEXT("RotR"), Obj.Rotation.Roll);
        ObjJson->SetNumberField(TEXT("ScaleX"), Obj.Scale.X);
        ObjJson->SetNumberField(TEXT("ScaleY"), Obj.Scale.Y);
        ObjJson->SetNumberField(TEXT("ScaleZ"), Obj.Scale.Z);
        ObjectsArray.Add(MakeShareable(new FJsonValueObject(ObjJson)));
    }
    RootObj->SetArrayField(TEXT("Objects"), ObjectsArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    if (!FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer))
    {
        return false;
    }

    if (FFileHelper::SaveStringToFile(OutputString, *FilePath))
    {
        UE_LOG(LogTemp, Log, TEXT("[UGC] 地图已保存: %s"), *FilePath);
        return true;
    }
    return false;
}

bool UEOUGCManager::LoadMap(const FString& MapName, FUGCMapData& OutMapData)
{
    FString SafeName = SanitizeFileName(MapName);
    FString FilePath = GetPlayerMapsDir() / SafeName + TEXT(".eomap");

    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *FilePath))
    {
        return false;
    }

    TSharedPtr<FJsonObject> RootObj;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);
    if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
    {
        return false;
    }

    OutMapData.MapName = RootObj->GetStringField(TEXT("MapName"));
    OutMapData.Author = RootObj->GetStringField(TEXT("Author"));
    OutMapData.Description = RootObj->GetStringField(TEXT("Description"));
    FDateTime::Parse(RootObj->GetStringField(TEXT("CreatedTime")), OutMapData.CreatedTime);
    FDateTime::Parse(RootObj->GetStringField(TEXT("ModifiedTime")), OutMapData.ModifiedTime);

    const TArray<TSharedPtr<FJsonValue>>& ObjectsArray = RootObj->GetArrayField(TEXT("Objects"));
    for (const TSharedPtr<FJsonValue>& Val : ObjectsArray)
    {
        TSharedPtr<FJsonObject> ObjJson = Val->AsObject();
        if (!ObjJson.IsValid()) continue;

        FUGCObjectData ObjData;
        ObjData.ObjectClassPath = ObjJson->GetStringField(TEXT("Class"));
        ObjData.Location = FVector(
            ObjJson->GetNumberField(TEXT("LocX")),
            ObjJson->GetNumberField(TEXT("LocY")),
            ObjJson->GetNumberField(TEXT("LocZ"))
        );
        ObjData.Rotation = FRotator(
            ObjJson->GetNumberField(TEXT("RotP")),
            ObjJson->GetNumberField(TEXT("RotY")),
            ObjJson->GetNumberField(TEXT("RotR"))
        );
        ObjData.Scale = FVector(
            ObjJson->GetNumberField(TEXT("ScaleX")),
            ObjJson->GetNumberField(TEXT("ScaleY")),
            ObjJson->GetNumberField(TEXT("ScaleZ"))
        );
        OutMapData.Objects.Add(ObjData);
    }

    return true;
}

bool UEOUGCManager::DeleteMap(const FString& MapName)
{
    FString SafeName = SanitizeFileName(MapName);
    FString FilePath = GetPlayerMapsDir() / SafeName + TEXT(".eomap");
    return IFileManager::Get().Delete(*FilePath);
}

TArray<FString> UEOUGCManager::ListPlayerMaps() const
{
    TArray<FString> Maps;
    TArray<FString> Files;
    IFileManager::Get().FindFiles(Files, *(GetPlayerMapsDir() / TEXT("*.eomap")), true, false);
    for (const FString& File : Files)
    {
        Maps.Add(FPaths::GetBaseFilename(File));
    }
    return Maps;
}

bool UEOUGCManager::MapExists(const FString& MapName) const
{
    FString SafeName = SanitizeFileName(MapName);
    FString FilePath = GetPlayerMapsDir() / SafeName + TEXT(".eomap");
    return FPaths::FileExists(FilePath);
}

bool UEOUGCManager::ExportMap(const FString& MapName, const FString& ExportPath)
{
    FString SafeName = SanitizeFileName(MapName);
    FString SrcPath = GetPlayerMapsDir() / SafeName + TEXT(".eomap");
    if (!FPaths::FileExists(SrcPath)) return false;
    return IFileManager::Get().Copy(*ExportPath, *SrcPath) == COPY_OK;
}

bool UEOUGCManager::ImportMap(const FString& ImportPath)
{
    if (!FPaths::FileExists(ImportPath)) return false;
    FString FileName = FPaths::GetCleanFilename(ImportPath);
    FString DestPath = GetPlayerMapsDir() / FileName;
    return IFileManager::Get().Copy(*DestPath, *ImportPath) == COPY_OK;
}

bool UEOUGCManager::SaveModel(const TArray<uint8>& ModelData, const FString& ModelName)
{
    FString SafeName = SanitizeFileName(ModelName);
    if (SafeName.IsEmpty()) return false;
    FString FilePath = GetPlayerModelsDir() / SafeName + TEXT(".eomdl");
    return FFileHelper::SaveArrayToFile(ModelData, *FilePath);
}

bool UEOUGCManager::LoadModel(const FString& ModelName, TArray<uint8>& OutModelData)
{
    FString SafeName = SanitizeFileName(ModelName);
    FString FilePath = GetPlayerModelsDir() / SafeName + TEXT(".eomdl");
    return FFileHelper::LoadFileToArray(OutModelData, *FilePath);
}

TArray<FString> UEOUGCManager::ListPlayerModels() const
{
    TArray<FString> Models;
    TArray<FString> Files;
    IFileManager::Get().FindFiles(Files, *(GetPlayerModelsDir() / TEXT("*.eomdl")), true, false);
    for (const FString& File : Files)
    {
        Models.Add(FPaths::GetBaseFilename(File));
    }
    return Models;
}

void UEOUGCManager::EnterEditMode()
{
    bEditMode = true;
    UE_LOG(LogTemp, Log, TEXT("[UGC] 进入编辑模式"));
}

void UEOUGCManager::ExitEditMode()
{
    bEditMode = false;
    UE_LOG(LogTemp, Log, TEXT("[UGC] 退出编辑模式"));
}

bool UEOUGCManager::EnsureDirExists(const FString& Dir) const
{
    if (!FPaths::DirectoryExists(Dir))
    {
        return IFileManager::Get().MakeDirectory(*Dir, true);
    }
    return true;
}

FString UEOUGCManager::SanitizeFileName(const FString& Name) const
{
    FString Result = Name;
    Result.RemoveSpacesInline();
    Result.ReplaceInline(TEXT("\\"), TEXT("_"));
    Result.ReplaceInline(TEXT("/"), TEXT("_"));
    Result.ReplaceInline(TEXT(":"), TEXT("_"));
    Result.ReplaceInline(TEXT("*"), TEXT("_"));
    Result.ReplaceInline(TEXT("?"), TEXT("_"));
    Result.ReplaceInline(TEXT("\""), TEXT("_"));
    Result.ReplaceInline(TEXT("<"), TEXT("_"));
    Result.ReplaceInline(TEXT(">"), TEXT("_"));
    Result.ReplaceInline(TEXT("|"), TEXT("_"));
    return Result;
}
