// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MusicInteractible.generated.h"

UENUM(BlueprintType)
enum EReactType
{
	Low,
	MidLow,
	MidHigh,
	High,
	Beat,
	Kick,
	Snare,
	HiHat,
};

UCLASS()
class SYNESTHESIA_API AMusicInteractible : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AMusicInteractible();

	UPROPERTY(BlueprintReadOnly)
	class AAudioSpectrum* Spectrum;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnUpdateMusic"))
	void OnUpdateMusic(FVector4 individuals1, FVector4 individuals2, FVector4 individuals3, FVector4 individuals4);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnUpdateLow"))
	void OnUpdateLow(float min, float average, float max, FVector4 individuals);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnUpdateMidLow"))
	void OnUpdateMidLow(float min, float average, float max, FVector4 individuals);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnUpdateMidHigh"))
	void OnUpdateMidHigh(float min, float average, float max, FVector4 individuals);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnUpdateHigh"))
	void OnUpdateHigh(float min, float average, float max, FVector4 individuals);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnBeatUpdate"))
	void OnBeatUpdate(bool isBeat);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnKickUpdate"))
	void OnKickUpdate(bool isKick);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSnareUpdate"))
	void OnSnareUpdate(bool isSnare);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnHiHatUpdate"))
	void OnHiHatUpdate(bool isHiHat);
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnMusicPause"))
	void OnMusicPause(bool isPaused);
};
