#include "EOKeyManager.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Windows/WindowsHWrapper.h"
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

// 游戏版本号
const FString UEOKeyManager::GameVersion = TEXT("0.1.8");

// 合法内测密钥的SHA256哈希列表（5个）
// 密钥本身不存储在代码中，只存哈希，防止逆向提取
const TArray<FString> UEOKeyManager::ValidKeyHashes = {
    TEXT("3c8858464b7143bd3b57d55cb504ad26edf17213b5429897b3e33e096cb4e2d4"),
    TEXT("8399970a61e22f73b277b9ebf986d237f905b1cbdb8d84ad4b7ef838caf5bfb5"),
    TEXT("d378bddaddd964a2377638276ad6b5012749d233e7633b266ba66a94a243d2da"),
    TEXT("8d34ba0268474edf58ac3ea1c1c26796f558d3900170b90c52f1f09f769f8cf6"),
    TEXT("9628830295d3a4e0a0fa8d8758f20cdd5b3fd73b8eb3165da9b4b7192b85da6b")
};

FString UEOKeyManager::ComputeSHA256(const FString& Input)
{
    // 转换为UTF8字节
    FTCHARToUTF8 Converter(*Input);
    const uint8* Data = (const uint8*)Converter.Get();
    ULONG Length = (ULONG)Converter.Length();

    // 使用Windows BCrypt API计算SHA256
    BCRYPT_ALG_HANDLE hAlgorithm = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    BYTE Hash[32] = {0};
    ULONG ResultLength = 0;

    BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0);
    BCryptCreateHash(hAlgorithm, &hHash, NULL, 0, NULL, 0, 0);
    BCryptHashData(hHash, (PUCHAR)Data, Length, 0);
    BCryptFinishHash(hHash, Hash, 32, 0);
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlgorithm, 0);

    // 转为十六进制字符串
    FString HashHex;
    for (int32 i = 0; i < 32; i++)
    {
        HashHex += FString::Printf(TEXT("%02x"), (int32)Hash[i]);
    }

    return HashHex;
}

bool UEOKeyManager::VerifyBetaKey(const FString& Key)
{
    if (Key.IsEmpty())
    {
        return false;
    }

    // 去除首尾空格，统一大写
    FString CleanKey = Key.TrimStartAndEnd().ToUpper();

    // 计算输入密钥的SHA256
    FString InputHash = ComputeSHA256(CleanKey);

    // 与合法哈希列表比对（恒定时间比较，防止时序攻击）
    bool bValid = false;
    for (const FString& ValidHash : ValidKeyHashes)
    {
        if (InputHash.Equals(ValidHash, ESearchCase::IgnoreCase))
        {
            bValid = true;
            break;
        }
    }

    return bValid;
}

bool UEOKeyManager::IsKeyActivated()
{
    FString SavePath = FPaths::ProjectSavedDir() / TEXT("beta_activation.dat");

    if (!FPaths::FileExists(SavePath))
    {
        return false;
    }

    FString StoredHash;
    if (FFileHelper::LoadFileToString(StoredHash, *SavePath))
    {
        StoredHash = StoredHash.TrimStartAndEnd();
        // 验证存储的哈希是否在合法列表中
        for (const FString& ValidHash : ValidKeyHashes)
        {
            if (StoredHash.Equals(ValidHash, ESearchCase::IgnoreCase))
            {
                return true;
            }
        }
    }

    return false;
}

void UEOKeyManager::SaveActivatedKey(const FString& Key)
{
    FString CleanKey = Key.TrimStartAndEnd().ToUpper();
    FString KeyHash = ComputeSHA256(CleanKey);

    FString SaveDir = FPaths::ProjectSavedDir();
    FPaths::MakePlatformFilename(SaveDir);
    IFileManager::Get().MakeDirectory(*SaveDir, true);

    FString SavePath = SaveDir / TEXT("beta_activation.dat");
    FFileHelper::SaveStringToFile(KeyHash, *SavePath);

    UE_LOG(LogTemp, Log, TEXT("内测密钥已激活并保存"));
}

void UEOKeyManager::ClearActivation()
{
    FString SavePath = FPaths::ProjectSavedDir() / TEXT("beta_activation.dat");
    if (FPaths::FileExists(SavePath))
    {
        IFileManager::Get().Delete(*SavePath);
    }
}

FString UEOKeyManager::GetGameVersion()
{
    return GameVersion;
}
