// Fill out your copyright notice in the Description page of Project Settings.

#include "Medkit.h"


// Sets default values
AMedkit::AMedkit()
{
	
	OnActorBeginOverlap.AddDynamic(this, &AMedkit::OnOverlap);
}

void AMedkit::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor) 
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		MyCharacter = Cast<AFPSCharacter>(OtherActor);
		if (MyCharacter && MyCharacter->GetHealth() < 1.0f)
		{
			MyCharacter->UpdateHealth(100.0f);
			Destroy();
		}
	}
}


