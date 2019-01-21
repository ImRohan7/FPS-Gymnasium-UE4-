// Fill out your copyright notice in the Description page of Project Settings.

#include "CampFIre.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
ACampFIre::ACampFIre()
{
	MyBoxComponent = CreateDefaultSubobject<UBoxComponent>((TEXT("My Box Component")));
	MyBoxComponent->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	RootComponent = MyBoxComponent;

	Fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("My Fire"));
	Fire->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Fire->SetupAttachment(RootComponent);
	
	MyBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACampFIre::OnOverlapBegin);
	MyBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACampFIre::OnOverlapEnd);

	bCanApplyDamage = false;
}

void ACampFIre::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		bCanApplyDamage = true;
		MyCharacter = Cast<AActor>(OtherActor);
		MyHit = SweepResult;
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ACampFIre::ApplyFireDamage, 2.2f, true, 0.0f);
	}
}

void ACampFIre::OnOverlapEnd( UPrimitiveComponent* OverlappedComp,  AActor* OtherActor,  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bCanApplyDamage = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ACampFIre::ApplyFireDamage()
{
	
	if (bCanApplyDamage)
	{UE_LOG(LogTemp, Warning, TEXT("Applying damage"));
		UGameplayStatics::ApplyPointDamage(MyCharacter, 200.0f, GetActorLocation(), MyHit, nullptr, this, FireDamageType);
	}
}
