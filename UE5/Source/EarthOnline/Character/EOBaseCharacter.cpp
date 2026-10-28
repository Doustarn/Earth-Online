#include "EOBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "InputCoreTypes.h"

AEOBaseCharacter::AEOBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    PlayerName = TEXT("玩家");
    PlayerLevel = 1;
    Health = 100.0f;
    MaxHealth = 100.0f;
    Stamina = 100.0f;
    MaxStamina = 100.0f;
    bIsSprinting = false;

    BaseWalkSpeed = 600.0f;
    SprintSpeed = 900.0f;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
    GetCharacterMovement()->JumpZVelocity = 500.0f;
    GetCharacterMovement()->AirControl = 0.3f;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
}

void AEOBaseCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AEOBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AEOBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AEOBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AEOBaseCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AEOBaseCharacter::LookUp);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AEOBaseCharacter::JumpPressed);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AEOBaseCharacter::SprintStart);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AEOBaseCharacter::SprintEnd);

    // —— 手柄支持（主机级体验）——
    PlayerInputComponent->BindAxisKey(EKeys::Gamepad_LeftX, this, &AEOBaseCharacter::MoveRight);
    PlayerInputComponent->BindAxisKey(EKeys::Gamepad_LeftY, this, &AEOBaseCharacter::MoveForward);
    PlayerInputComponent->BindAxisKey(EKeys::Gamepad_RightX, this, &AEOBaseCharacter::Turn);
    PlayerInputComponent->BindAxisKey(EKeys::Gamepad_RightY, this, &AEOBaseCharacter::LookUp);
    PlayerInputComponent->BindKey(EKeys::Gamepad_FaceButton_Bottom, IE_Pressed, this, &AEOBaseCharacter::JumpPressed);
    PlayerInputComponent->BindKey(EKeys::Gamepad_LeftShoulder, IE_Pressed, this, &AEOBaseCharacter::SprintStart);
    PlayerInputComponent->BindKey(EKeys::Gamepad_LeftShoulder, IE_Released, this, &AEOBaseCharacter::SprintEnd);
}

void AEOBaseCharacter::MoveForward(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AEOBaseCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AEOBaseCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AEOBaseCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AEOBaseCharacter::JumpPressed()
{
    Jump();
}

void AEOBaseCharacter::SprintStart()
{
    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AEOBaseCharacter::SprintEnd()
{
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void AEOBaseCharacter::TakeDamage(float Damage)
{
    Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
}

void AEOBaseCharacter::Heal(float Amount)
{
    Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
}

void AEOBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AEOBaseCharacter, PlayerName);
    DOREPLIFETIME(AEOBaseCharacter, PlayerLevel);
    DOREPLIFETIME(AEOBaseCharacter, Health);
    DOREPLIFETIME(AEOBaseCharacter, Stamina);
}
