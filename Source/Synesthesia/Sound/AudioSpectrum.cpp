// Fill out your copyright notice in the Description page of Project Settings.


#include "AudioSpectrum.h"
#include <cmath>
#include <fstream>

#include "Components/AudioComponent.h"
#include "ConstantQNRT.h"
#include "AudioAnalyzerNRT.h"
#include "AudioAnalysisToolsLibrary.h"
#include "RuntimeAudioImporterLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAudioSpectrum::AAudioSpectrum()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	RootComponent = AudioComponent;
}

// Called when the game starts or when spawned
void AAudioSpectrum::BeginPlay()
{
	Super::BeginPlay();

}

void AAudioSpectrum::FindAllPlaylistSongs()
{
	FString playlistPath = GetPlaylistFolderPath();

	TArray<FString> extensions;
	extensions.Add(TEXT("*.mp3"));
	extensions.Add(TEXT("*.wav"));
	extensions.Add(TEXT("*.flac"));
	extensions.Add(TEXT("*.ogg"));
	extensions.Add(TEXT("*.bink"));

	IFileManager& fileManager = IFileManager::Get();

	// Check if the directory exists, if not, create it
	if (!fileManager.DirectoryExists(*playlistPath))
	{
		fileManager.MakeDirectory(*playlistPath, true);
	}

	// Loop through each extension and find files
	for (const FString& ext : extensions)
	{
		TArray<FString> fileNames;
		fileManager.FindFiles(fileNames, *playlistPath, *ext);

		PlaylistSongs.Append(fileNames);
	}

}

void AAudioSpectrum::PlayNextSong()
{
	if (PlaylistSongs.Num() == 0)
		return;

	CurrentSongIndex = (CurrentSongIndex + 1) % PlaylistSongs.Num();

	if (CurrentSongIndex < PlaylistSongs.Num())
	{
		ImportAndPlaySound(GetPlaylistFolderPath() + PlaylistSongs[CurrentSongIndex], true);
	}

	OnMusicPaused.Broadcast(bIsPlaying);
}

void AAudioSpectrum::PlayPreviousSong()
{
	CurrentSongIndex = CurrentSongIndex - 1;

	if (CurrentSongIndex < 0)
		CurrentSongIndex = PlaylistSongs.Num() - 1;

	if (CurrentSongIndex < PlaylistSongs.Num())
	{
		ImportAndPlaySound(GetPlaylistFolderPath() + PlaylistSongs[CurrentSongIndex], true);
	}

	OnMusicPaused.Broadcast(bIsPlaying);
}

FString AAudioSpectrum::GetPlaylistFolderPath()
{
	return FPaths::ProjectContentDir() + TEXT("/Playlist/");
}

/*
void AAudioSpectrum::OnPlaybackPercentage(const USoundWave* PlayingSoundWave, const float PlaybackPercent)
{
	float positionInAudio = PlaybackPercent * SoundDuration;

	if (positionInAudio == 0)
		return;

	TArray<float> constantQ;
	ConstantQNRT->GetNormalizedChannelConstantQAtTime(positionInAudio, 0, constantQ);
	if (constantQ.Num() == 0)
		return;
	Amplitudes.SetNum(constantQ.Num());

	TArray<float> averages;
	TArray<float> minimums;
	TArray<float> maximums;
	TArray<FVector4> individuals;
	previousAverages.Init(0.0f, 4);
	averages.Init(0.0f, 4);

	{
		FVector4 initVal{ 0.0f, 0.0f, 0.0f, 0.0f };
		individuals.Init(initVal, 4);
	}

	minimums.Init(FLT_MAX, 4);
	maximums.Init(FLT_MIN, 4);
	const int partSize = constantQ.Num() / 4;
	for (int i = 0; i < constantQ.Num(); i++)
	{
		const size_t index = i / partSize;
		Amplitudes[i] = constantQ[i];
		averages[index] += constantQ[i];
		if (minimums[index] > constantQ[i])
			minimums[index] = constantQ[i];
		if (maximums[index] < constantQ[i])
			maximums[index] = constantQ[i];

		if (i % 4 == 0)
			individuals[index][i % 4] = (constantQ[i] + constantQ[i+1] + constantQ[i+2] + constantQ[i+3]) / 4.0f;
	}
	// Calculate the actual averages for each part
	for (int i = 0; i < averages.Num(); i++)
	{
		// Avoid division by zero in case partSize is 0
		if (partSize > 0)
		{
			averages[i] /= partSize;
		}
		averages[i] = UKismetMathLibrary::WeightedMovingAverage_Float(averages[i], previousAverages[i], SmoothWeight);
		previousAverages[i] = averages[i];
	}

	OnMusicUpdate.Broadcast(individuals);
	OnLowUpdate.Broadcast(minimums[0], averages[0], maximums[0], individuals[0]);
	OnMidLowUpdate.Broadcast(minimums[1], averages[1], maximums[1], individuals[1]);
	OnMidHighUpdate.Broadcast(minimums[2], averages[2], maximums[2], individuals[2]);
	OnHighUpdate.Broadcast(minimums[3], averages[3], maximums[3], individuals[3]);
}
	*/

void AAudioSpectrum::OnGeneratePCMData(const TArray<float>& PCMData)
{
	AudioAnalysisTools->ProcessAudioFrames(PCMData, true);
	OnBeatUpdate.Broadcast(AudioAnalysisTools->IsBeat(0));
}

void AAudioSpectrum::NormalizeArray(TArray<float>& Data)
{
	// Find the minimum and maximum values in PCMData for normalization
	float minVal = FLT_MAX;
	float maxVal = FLT_MIN;
	for (float sample : Data)
	{
		minVal = FMath::Min(minVal, sample);
		maxVal = FMath::Max(maxVal, sample);
	}

	TArray<float> normalizedData;
	// Check if the maxVal and minVal are the same to avoid division by zero
	if (maxVal == minVal)
	{
		normalizedData.Init(minVal, Data.Num());
	}
	else
	{
		// Normalized  Data
		normalizedData.Reserve(Data.Num());
		for (float sample : Data)
		{
			normalizedData.Add((sample - minVal) / (maxVal - minVal));
		}
	}
	Data = normalizedData;
}


void AAudioSpectrum::NormalizeArrayByValue(TArray<float>& Data, float value)
{
	TArray<float> normalizedData;
	// Check if the maxVal and minVal are the same to avoid division by zero
	if (value == 0)
	{
		normalizedData.Init(0, Data.Num());
	}
	else
	{
		// Normalized  Data
		normalizedData.Reserve(Data.Num());
		for (float sample : Data)
		{
			normalizedData.Add(sample / value);
		}
	}
	Data = normalizedData;
}

void AAudioSpectrum::CalcPart(	const int& partStart,
								const int& partEnd,
								float& averageAmplitude,
								const int& amplitudeUpdateInterval,
								size_t& k,
								int partIndex,
								TArray<float>& averages,
								TArray<float>& minimums,
								TArray<float>& maximums,
								TArray<FVector4>& individuals,
								const TArray<float>& Data)
{
	// Average of the amplitudes in each part
	float averagePart = 0.0f;

	// Calculate size of each segment to divide the data into four parts
	int segmentSize = (partEnd - partStart) / 4;
	float segmentAverage = 0.0f;
	int segmentCount = 0;

	for (size_t j = 0; j < partEnd - partStart; j++)
	{
		const size_t index = partStart + j;
		if (index >= Data.Num())
			break;

		const float value = Data[index] * AmplitudeMultiplier;
		averagePart += value;
		averageAmplitude += value;
		segmentAverage += value;

		if (minimums[partIndex] > value)
			minimums[partIndex] = value;
		if (maximums[partIndex] < value)
			maximums[partIndex] = value;

		if (index % amplitudeUpdateInterval == 0 && k < Amplitudes.Num())
		{
			// Should calculate the average
			averageAmplitude /= amplitudeUpdateInterval;
			Amplitudes[k] = averageAmplitude;

			// Calculate the Weighted Moving Average
			Amplitudes[k] = UKismetMathLibrary::WeightedMovingAverage_Float(Amplitudes[k], previousAmplitudes[k], SmoothWeight);
			previousAmplitudes[k] = Amplitudes[k];
			k++;
			averageAmplitude = 0.0f;
		}

		// Check if we've reached the end of the current segment
		if ((j + 1) % segmentSize == 0 || j == partEnd - partStart - 1)
		{
			if (segmentCount > 3)
				continue;
			// Calculate and store the average for the current segment
			individuals[partIndex][segmentCount] = segmentAverage / segmentSize;

			// Reset for the next segment
			segmentAverage = 0.0f;

			// Move to the next segment
			segmentCount++;
		}
	}

	averages[partIndex] = (averagePart / static_cast<float>(partEnd - partStart));
	averages[partIndex] = UKismetMathLibrary::WeightedMovingAverage_Float(averages[partIndex], previousAverages[partIndex], SmoothWeight);
	previousAverages[partIndex] = averages[partIndex];
}

void AAudioSpectrum::UpdateAmplitudes(const TArray<float>& Data)
{
	TArray<float> averages;
	TArray<float> minimums;
	TArray<float> maximums;
	TArray<FVector4> individuals;

	auto newArray = Data;
	newArray.RemoveAt(Data.Num() / 2, Data.Num() / 2);
	newArray.RemoveAt(newArray.Num() * 3 / 4, newArray.Num() / 4);

	if (NormalizeAmplitudeValues)
		NormalizeArrayByValue(newArray, NormalizationValue);

	previousAverages.Init(0.0f, 4);
	averages.Init(0.0f, 4);
	minimums.Init(FLT_MAX, 4);
	maximums.Init(FLT_MIN, 4);
	individuals.Init(FVector4(ForceInitToZero), 4);

	Amplitudes.Init(0.0f, AmplitudeNumber);
	previousAmplitudes.Init(0.0f, AmplitudeNumber);

	constexpr int partCount = 4;
	const size_t partSize = newArray.Num() / partCount;

	// Will do a average each n index of the PCMData to avoid using an array of 1024 floats
	const int amplitudeUpdateInterval = std::round((float)newArray.Num() / (float)FMath::Min(AmplitudeNumber, (uint32)newArray.Num()));

	size_t k = 0;
		// Average of the amplitudes within each AmplitudeNumber
	float averageAmplitude = 0.0f;
	// Bass (0Hz - 250Hz) id: 0 - 4
	CalcPart(0, 4, averageAmplitude, amplitudeUpdateInterval, k, 0,
		averages, minimums, maximums, individuals, newArray);

	// MidLow (250Hz - 1500Hz) id:  4 - 19
	CalcPart(4, 19, averageAmplitude, amplitudeUpdateInterval, k, 1,
		averages, minimums, maximums, individuals, newArray);

	// MidHigh (1500Hz - 4kHz) id: 19 - 51
	CalcPart(19, 51, averageAmplitude, amplitudeUpdateInterval, k, 2,
		averages, minimums, maximums, individuals, newArray);

	// High (4k+Hz) id: 51 - 256
	CalcPart(51, 256, averageAmplitude, amplitudeUpdateInterval, k, 3,
		averages, minimums, maximums, individuals, newArray);

	if (bIsPlaying)
	{
		OnMusicUpdate.Broadcast(individuals[0], individuals[1], individuals[2], individuals[3]);
		OnLowUpdate.Broadcast(minimums[0], averages[0], maximums[0], individuals[0]);
		OnMidLowUpdate.Broadcast(minimums[1], averages[1], maximums[1], individuals[1]);
		OnMidHighUpdate.Broadcast(minimums[2], averages[2], maximums[2], individuals[2]);
		OnHighUpdate.Broadcast(minimums[3], averages[3], maximums[3], individuals[3]);
	}
}

// Called every frame
void AAudioSpectrum::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAudioSpectrum::Play()
{
	bIsPlaying = true;
	AudioComponent->Play();
	AudioComponent->SetPaused(false);
}

void AAudioSpectrum::Pause(bool value)
{
	bIsPlaying = !value;
	AudioComponent->SetPaused(value);
	if (!AudioComponent->IsPlaying())
		Play();
	OnMusicPaused.Broadcast(value);
}

void AAudioSpectrum::SetSound(UImportedSoundWave* newSound)
{
	if (!newSound)
		return;
	AudioComponent->SetSound(newSound);
	ImportedSoundWave = newSound;
	SoundDuration = newSound->GetDuration();
}

void AAudioSpectrum::ImportMusic(FString path)
{
	if (!std::ifstream(*path).good())
	{
		FString name = FPaths::ProjectDir() + path;
		path = FPaths::ConvertRelativePathToFull(name);
	}

	RuntimeAudioImporter = URuntimeAudioImporterLibrary::CreateRuntimeAudioImporter();
	AudioAnalysisTools = UAudioAnalysisToolsLibrary::CreateAudioAnalysisTools();

	if (!IsValid(RuntimeAudioImporter))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create audio importer"));
		return;
	}

	RuntimeAudioImporter->OnProgressNative.AddWeakLambda(this, [](int32 Percentage)
		{
			UE_LOG(LogTemp, Log, TEXT("Audio importing percentage: %d"), Percentage);
		});

	RuntimeAudioImporter->OnResultNative.AddWeakLambda(this, [this](URuntimeAudioImporterLibrary* Importer, UImportedSoundWave* ImportedSoundWave, ERuntimeImportStatus Status)
		{
			if (Status == ERuntimeImportStatus::SuccessfulImport)
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully imported audio with sound wave %s"), *ImportedSoundWave->GetName());
				SetSound(ImportedSoundWave);
				ImportedSoundWave->OnGeneratePCMData.AddDynamic(this, &AAudioSpectrum::OnGeneratePCMData);
				Play();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to import audio"));
			}

			RuntimeAudioImporter = nullptr;
		});

	RuntimeAudioImporter->ImportAudioFromFile(path, ERuntimeAudioFormat::Auto);
}
