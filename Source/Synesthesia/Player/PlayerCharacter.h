// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class AAudioSpectrum;

UCLASS()
class SYNESTHESIA_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FPSCamera;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	float HorizontalSensivity = 10;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	float VerticalSensivity = 10;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	float InteractionDistance = 250;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float sprintSpeed = 800;

	APlayerController* playerController;

	AAudioSpectrum* spectrum;

	float baseMaxSpeed;
private:
	bool inMenu = false;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);


	void MoveRight(float Value);

	//UI Inputs
	void UIMoveUp();
	void UIMoveDown();
	void UIMoveLeft();
	void UIMoveRight();
	void Select();
	void PauseMusic();
	void PlayNextSong();
	void PlayPreviousSong();

	UFUNCTION(BlueprintCallable)
	void ToggleMenu();

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	void Sprint();

	void StopSprint();

	void QuitGame();

	// UI Methods
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUICursorUpDown(bool up);
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUICursorLeftRight(bool left);

	UFUNCTION(BlueprintImplementableEvent)
	void UISelect();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OpenMenu(bool value);
};