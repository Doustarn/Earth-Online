#include "EOLang.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "GenericPlatform/GenericPlatformFile.h"

int32 UEOLang::CurrentLang = 0;

FString UEOLang::T(const FString& CN, const FString& EN)
{
    return CurrentLang == 0 ? CN : EN;
}

void UEOLang::LoadLang()
{
    FString SaveDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Save"));
    IPlatformFile::GetPlatformPhysical().CreateDirectoryTree(*SaveDir);
    FString LangFile = FPaths::Combine(SaveDir, TEXT("lang.txt"));
    FString Str;
    if (FFileHelper::LoadFileToString(Str, *LangFile))
    {
        CurrentLang = Str.TrimStartAndEnd().Equals(TEXT("1")) ? 1 : 0;
    }
}

void UEOLang::SaveLang(int32 Lang)
{
    FString SaveDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Save"));
    IPlatformFile::GetPlatformPhysical().CreateDirectoryTree(*SaveDir);
    FString LangFile = FPaths::Combine(SaveDir, TEXT("lang.txt"));
    FFileHelper::SaveStringToFile(Lang == 1 ? TEXT("1") : TEXT("0"), *LangFile);
    CurrentLang = Lang;
}
