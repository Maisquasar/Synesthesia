// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioSpectrum.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMusicUpdate, FVector4, individuals1, FVector4, individuals2, FVector4, individuals3, FVector4, individuals4);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateBeat, bool, isBeat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateKick, bool, isKick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateSnare, bool, isSnare);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateHiHat, bool, isHiHat);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnLowUpdate, const float, min, const float, average, const float, max, FVector4, individuals);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMidLowUpdate, const float, min, const float, average, const float, max, FVector4, individuals);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnMidHighUpdate, const float, min, const float, average, const float, max, FVector4, individuals);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHighUpdate, const float, min, const float, average, const float, max, FVector4, individuals);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMusicPaused, bool, isPaused);


UCLASS()
class SYNESTHESIA_API AAudioSpectrum : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAudioSpectrum();

	UPROPERTY(BlueprintReadWrite)
	class UAudioComponent* AudioComponent;

	UPROPERTY(BlueprintReadOnly)
	float SoundDuration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString MusicFullPath;

	UPROPERTY(EditAnywhere)
	float AmplitudeMultiplier = 1;

	UPROPERTY(EditAnywhere)
	float NormalizationValue = 20;

	UPROPERTY(EditAnywhere)
	bool NormalizeAmplitudeValues = true;

	UPROPERTY(EditAnywhere)
	uint32 AmplitudeNumber = 32;

	UPROPERTY(BlueprintReadWrite)
	TArray<float> Amplitudes;

	UPROPERTY(BlueprintReadWrite)
	int CurrentSongIndex;
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> PlaylistSongs;

	// Events
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnMusicUpdate OnMusicUpdate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnLowUpdate OnLowUpdate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnMidLowUpdate OnMidLowUpdate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnMidHighUpdate OnMidHighUpdate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnHighUpdate OnHighUpdate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdateBeat OnBeatUpdate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdateKick OnKickUpdate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdateSnare OnSnareUpdate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnUpdateHiHat OnHiHatUpdate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnMusicPaused OnMusicPaused;


	UPROPERTY(BlueprintReadWrite)
	class URuntimeAudioImporterLibrary* RuntimeAudioImporter;

	UPROPERTY(BlueprintReadWrite)
	class UAudioAnalysisToolsLibrary* AudioAnalysisTools;

	UPROPERTY(EditAnywhere)
	float SmoothWeight = 0.27f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsPlaying = false;

	UPROPERTY(BlueprintReadWrite)
	class UImportedSoundWave* ImportedSoundWave;

private:
	// the previous averages of the amplitudes to calculate smooth
	TArray<float> previousAverages;

	TArray<float> previousAmplitudes;

	TArray<FVector4> previousIndividuals;

	TArray<float> normalizedPCMData;

	void CalcPart(const int& partStart,
					const int& partEnd,
					float& averageAmplitude,
					const int& amplitudeUpdateInterval,
					size_t& k,
					int partIndex,
					TArray<float>& averages,
					TArray<float>& minimums,
					TArray<float>& maximums,
					TArray<FVector4>& individuals,
					const TArray<float>& Data);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGeneratePCMData(const TArray<float>& PCMData);

	void NormalizeArray(TArray<float>& AllPCMData);

	void NormalizeArrayByValue(TArray<float>& AllPCMData, float value);

	UFUNCTION(BlueprintCallable)
	void UpdateAmplitudes(const TArray<float>& Data);

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Play();

	UFUNCTION(BlueprintCallable)
	void Pause(bool value);

	UFUNCTION(BlueprintCallable)
	void SetSound(class UImportedSoundWave* newSound);

	UFUNCTION(BlueprintCallable)
	void ImportMusic(FString path);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ImportAndPlaySound(const FString& musicPath, bool shouldPlaySong);

	UFUNCTION(BlueprintCallable)
	FString GetPlaylistFolderPath();

	UFUNCTION(BlueprintCallable)
	void FindAllPlaylistSongs();

	UFUNCTION(BlueprintCallable)
	void PlayNextSong();

	UFUNCTION(BlueprintCallable)
	void PlayPreviousSong();
};
