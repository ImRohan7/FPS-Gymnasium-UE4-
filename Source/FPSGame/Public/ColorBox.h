// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSCharacter.h"
#include "ColorBox.generated.h"


UCLASS()
class FPSGAME_API AColorBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AColorBox();

	UPROPERTY(EditAnywhere)
		AFPSCharacter* MyCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorBox")
	bool IsColored;


public:	

	UFUNCTION(BlueprintCallable, Category = "ColorBox")
	void ColorMe();
	
};
