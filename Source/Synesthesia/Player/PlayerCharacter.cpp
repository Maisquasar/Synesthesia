// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharacter.h"
#include "../Sound/AudioSpectrum.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPS Camera"));
	FPSCamera->SetupAttachment(RootComponent);
	FPSCamera->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	playerController = GetWorld()->GetFirstPlayerController();

	baseMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	spectrum = Cast<AAudioSpectrum>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioSpectrum::StaticClass()));
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);

	PlayerInputComponent->BindAction("MenuUp", IE_Pressed, this, &APlayerCharacter::UIMoveUp);
	PlayerInputComponent->BindAction("MenuDown", IE_Pressed, this, &APlayerCharacter::UIMoveDown);
	PlayerInputComponent->BindAction("MenuRight", IE_Repeat, this, &APlayerCharacter::UIMoveRight);
	PlayerInputComponent->BindAction("MenuLeft", IE_Repeat, this, &APlayerCharacter::UIMoveLeft);
	PlayerInputComponent->BindAction("Select", IE_Pressed, this, &APlayerCharacter::Select);
	PlayerInputComponent->BindAction("ToggleMenu", IE_Pressed, this, &APlayerCharacter::ToggleMenu);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &APlayerCharacter::PauseMusic);
	PlayerInputComponent->BindAction("PlayNextSong", IE_Pressed, this, &APlayerCharacter::PlayNextSong);
	PlayerInputComponent->BindAction("PlayPreviousSong", IE_Pressed, this, &APlayerCharacter::PlayPreviousSong);

	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

}

void APlayerCharacter::MoveForward(float Value)
{
	if (inMenu)
		return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (inMenu)
		return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::UIMoveRight()
{
	if (!inMenu)
		return;
	UpdateUICursorLeftRight(false);
}

void APlayerCharacter::UIMoveLeft()
{
	if (!inMenu)
		return;
	UpdateUICursorLeftRight(true);
}

void APlayerCharacter::UIMoveUp()
{
	if (!inMenu)
		return;
	UpdateUICursorUpDown(true);
}

void APlayerCharacter::UIMoveDown()
{
	if (!inMenu)
		return;
	UpdateUICursorUpDown(false);
}

void APlayerCharacter::Select()
{
	if (!inMenu)
		return;
	UISelect();
}

void APlayerCharacter::ToggleMenu()
{
	inMenu = !inMenu;
	OpenMenu(inMenu);
}

void APlayerCharacter::PauseMusic()
{
	spectrum->Pause(spectrum->bIsPlaying);
}

void APlayerCharacter::PlayNextSong()
{
	spectrum->PlayNextSong();
}

void APlayerCharacter::PlayPreviousSong()
{
	spectrum->PlayPreviousSong();
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	if (inMenu)
		return;
	AddControllerYawInput(Rate * HorizontalSensivity * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	if (inMenu)
		return;
	float pitch = Rate * VerticalSensivity * GetWorld()->GetDeltaSeconds();
	AddControllerPitchInput(pitch);
}

void APlayerCharacter::QuitGame()
{
	Cast<APlayerController>(GetController())->ConsoleCommand("Quit");
}