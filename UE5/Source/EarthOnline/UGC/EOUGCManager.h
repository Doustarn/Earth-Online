// Immortal Dao Online - UGC玩家自建内容管理器
// 支持玩家自建地图、建模、保存/加载
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EOUGCManager.generated.h"

class UEOUGCMapData;

USTRUCT(BlueprintType)
struct FUGCObjectData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString ObjectClassPath;

    UPROPERTY(BlueprintReadWrite)
    FVector Location;

    UPROPERTY(BlueprintReadWrite)
    FRotator Rotation;

    UPROPERTY(BlueprintReadWrite)
    FVector Scale;

    UPROPERTY(BlueprintReadWrite)
    TArray<uint8> CustomData;

    FUGCObjectData()
        : Location(FVector::ZeroVector)
        , Rotation(FRotator::ZeroRotator)
        , Scale(FVector(1, 1, 1))
    {}
};

USTRUCT(BlueprintType)
struct FUGCMapData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString MapName;

    UPROPERTY(BlueprintReadWrite)
    FString Author;

    UPROPERTY(BlueprintReadWrite)
    FString Description;

    UPROPERTY(BlueprintReadWrite)
    FDateTime CreatedTime;

    UPROPERTY(BlueprintReadWrite)
    FDateTime ModifiedTime;

    UPROPERTY(BlueprintReadWrite)
    TArray<FUGCObjectData> Objects;

    UPROPERTY(BlueprintReadWrite)
    TArray<uint8> TerrainData;

    UPROPERTY(BlueprintReadWrite)
    TArray<uint8> LightData;

    FUGCMapData()
        : CreatedTime(FDateTime::UtcNow())
        , ModifiedTime(FDateTime::UtcNow())
    {}
};

UCLASS(BlueprintType)
class EARTHONLINE_API UEOUGCManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // 获取UGC目录路径
    UFUNCTION(BlueprintCallable, Category = "UGC")
    FString GetUGCRootDir() const;

    // 获取玩家地图目录
    UFUNCTION(BlueprintCallable, Category = "UGC")
    FString GetPlayerMapsDir() const;

    // 获取玩家模型目录
    UFUNCTION(BlueprintCallable, Category = "UGC")
    FString GetPlayerModelsDir() const;

    // 保存地图
    UFUNCTION(BlueprintCallable, Category = "UGC")
    bool SaveMap(const FUGCMapData& MapData, const FString& MapName);

    // 加载地图
    UFUNCTION(BlueprintCallable, Category = "UGC")
    bool LoadMap(const FString& MapName, FUGCMapData& OutMapData);

    // 删除地图
    UFUNCTION(BlueprintCallable, Category = "UGC")
    bool DeleteMap(const FString& MapName);

    // 列出所有玩家地图
    UFUNCTION(BlueprintCallable, Category = "UGC")
    TArray<FString> ListPlayerMaps() const;

    // 检查地图是否存在
    UFUNCTION(BlueprintCallable, Category = "UGC")
    bool MapExists(const FString& MapName) const;

    // 导出地图为分享文件
    UFUNCTION(BlueprintCallable, Category = "UGC")
    bool ExportMap(const FString& MapName, const FString& ExportPath);

    // 导入分享地图
    UFUNCTION(BlueprintCallable, Category = "UGC")
    bool ImportMap(const FString& ImportPath);

    // 保存自定义模型
    UFUNCTION(BlueprintCallable, Category = "UGC")
    bool SaveModel(const TArray<uint8>& ModelData, const FString& ModelName);

    // 加载自定义模型
    UFUNCTION(BlueprintCallable, Category = "UGC")
    bool LoadModel(const FString& ModelName, TArray<uint8>& OutModelData);

    // 列出所有玩家模型
    UFUNCTION(BlueprintCallable, Category = "UGC")
    TArray<FString> ListPlayerModels() const;

    // 进入编辑模式
    UFUNCTION(BlueprintCallable, Category = "UGC")
    void EnterEditMode();

    // 退出编辑模式
    UFUNCTION(BlueprintCallable, Category = "UGC")
    void ExitEditMode();

    // 是否在编辑模式
    UFUNCTION(BlueprintCallable, Category = "UGC")
    bool IsInEditMode() const { return bEditMode; }

    // 当前编辑的地图名
    UPROPERTY(BlueprintReadOnly, Category = "UGC")
    FString CurrentEditingMap;

private:
    bool bEditMode = false;

    bool EnsureDirExists(const FString& Dir) const;
    FString SanitizeFileName(const FString& Name) const;
};
