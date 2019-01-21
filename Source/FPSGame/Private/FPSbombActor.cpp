// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSbombActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AFPSbombActor::AFPSbombActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	RootComponent = MeshComp;

	ExplodeDelay = 2.0f;
}

// Called when the game starts or when spawned
void AFPSbombActor::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle Explode_TimerHandle;
	GetWorldTimerManager().SetTimer(Explode_TimerHandle, this, &AFPSbombActor::Explode, ExplodeDelay);
	
	MaterialInst = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
	if (MaterialInst) 
	{
		CurrentColor = MaterialInst->K2_GetVectorParameterValue("Color");
	}

	TargetColor = FLinearColor::MakeRandomColor();
}

// Called every frame
void AFPSbombActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MaterialInst)
	{
		float Progress = (GetWorld()->TimeSeconds - CreationTime) / ExplodeDelay;
		FLinearColor newColor = FLinearColor::LerpUsingHSV(CurrentColor, TargetColor, Progress);

		MaterialInst->SetVectorParameterValue("Color",newColor);
	}

}

void AFPSbombActor::Explode() 
{
	UGameplayStatics::SpawnEmitterAtLocation(this, Explosiontemplate, GetActorLocation());

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	// set collision area
	FCollisionShape CollShape;
	CollShape.SetSphere(600.0f);

	TArray<FOverlapResult> OutOverlaps;
	GetWorld()->OverlapMultiByObjectType(OutOverlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	for (FOverlapResult Result : OutOverlaps)
	{
		UPrimitiveComponent* Overlap = Result.GetComponent();
		if (Overlap && Overlap->IsSimulatingPhysics())
		{
			UMaterialInstanceDynamic* MatInst = Overlap->CreateAndSetMaterialInstanceDynamic(0);
			if (MatInst)
			{
				MatInst->SetVectorParameterValue("Color", TargetColor);
			}
		}
	}


	Destroy();
}