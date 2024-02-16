// Fill out your copyright notice in the Description page of Project Settings.

#include "MusicInteractible.h"
#include "Sound/AudioSpectrum.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMusicInteractible::AMusicInteractible()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMusicInteractible::BeginPlay()
{
	Super::BeginPlay();

	Spectrum = Cast<AAudioSpectrum>(UGameplayStatics::GetActorOfClass(GetWorld(), AAudioSpectrum::StaticClass()));

	if (!Spectrum)
		return;

	Spectrum->OnMusicUpdate.AddDynamic(this, &AMusicInteractible::OnUpdateMusic);
	Spectrum->OnLowUpdate.AddDynamic(this, &AMusicInteractible::OnUpdateLow);
	Spectrum->OnMidLowUpdate.AddDynamic(this, &AMusicInteractible::OnUpdateMidLow);
	Spectrum->OnMidHighUpdate.AddDynamic(this, &AMusicInteractible::OnUpdateMidHigh);
	Spectrum->OnHighUpdate.AddDynamic(this, &AMusicInteractible::OnUpdateHigh);
	Spectrum->OnBeatUpdate.AddDynamic(this, &AMusicInteractible::OnBeatUpdate);
	Spectrum->OnKickUpdate.AddDynamic(this, &AMusicInteractible::OnKickUpdate);
	Spectrum->OnSnareUpdate.AddDynamic(this, &AMusicInteractible::OnSnareUpdate);
	Spectrum->OnHiHatUpdate.AddDynamic(this, &AMusicInteractible::OnHiHatUpdate);
	Spectrum->OnMusicPaused.AddDynamic(this, &AMusicInteractible::OnMusicPause);
}

// Called every frame
void AMusicInteractible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
