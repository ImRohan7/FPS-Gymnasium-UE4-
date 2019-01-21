// Fill out your copyright notice in the Description page of Project Settings.

#include "ColorBox.h"


// Sets default values
AColorBox::AColorBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AColorBox::ColorMe() {
	IsColored = true;
}
