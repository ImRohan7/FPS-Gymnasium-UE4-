// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"
#include "FPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "FPSbombActor.h"
#include "TimerManager.h"

//#define LOG(X) UE_LOG(LogTemp, Warning, TEXT(X))  // lazy

AFPSCharacter::AFPSCharacter()
{
	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->RelativeLocation = FVector(0, 0, BaseEyeHeight); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1PComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	Mesh1PComponent->SetupAttachment(CameraComponent);
	Mesh1PComponent->CastShadow = false;
	Mesh1PComponent->RelativeRotation = FRotator(2.0f, -15.0f, 5.0f);
	Mesh1PComponent->RelativeLocation = FVector(0, 0, -160.0f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(Mesh1PComponent, "GripPoint");
}

void AFPSCharacter::BeginPlay()
{
	// calling base class
	Super::BeginPlay();

//-- Can be done in Contsructor as well
//-- Initialize health

	FullHealth = 1000.0f;
	Health = FullHealth;
	HealthPercentage = 1.0f;
	bCanBeDamaged = true;

//-- Initialize Magic
	FullMagic = 100.0f;
	Magic = FullMagic;
	MagicPercentage = 1.0f;
	PreviousMagic = MagicPercentage;
	MagicValue = 0.0f;
	bCanUseMagic = true;


	if (MagicCurve)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;
		UE_LOG(LogTemp, Warning, TEXT(" Magic"));
		TimelineCallback.BindUFunction(this, FName("SetMagicValue"));
		TimelineFinishedCallback.BindUFunction(this, FName("SetMagicState"));
		MyTimeline.AddInterpFloat(MagicCurve, TimelineCallback);
		MyTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);

	}
}

//-- 
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float x = Health;
	//UE_LOG(LogTemp, Warning,TEXT("%f"), x);
	MyTimeline.TickTimeline(DeltaTime);

}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);

	PlayerInputComponent->BindAction("SpawnBomb", IE_Pressed, this, &AFPSCharacter::SpawnBomb);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}


void AFPSCharacter::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("Magic: %f"), Magic);
	UE_LOG(LogTemp, Warning, TEXT("Magic percentage: %f"), MagicPercentage);
	UE_LOG(LogTemp, Warning, TEXT("Magic Value : %f"), MagicValue);
	UE_LOG(LogTemp, Warning, TEXT("Magic State : "), bCanUseMagic);

	// try and fire a projectile
	//--
	if (ProjectileClass != NULL && !FMath::IsNearlyZero(Magic, 0.001f) && bCanUseMagic && Health > 0)
	{
		FVector MuzzleLocation = FP_Gun->GetSocketLocation("Muzzle");
		FRotator MuzzleRotation = FP_Gun->GetSocketRotation("Muzzle");

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
	
		// try and play the sound if specified
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1PComponent->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->PlaySlotAnimationAsDynamicMontage(FireAnimation, "Arms", 0.0f);
			}
		}

		//--
		MyTimeline.Stop();
		GetWorldTimerManager().ClearTimer(MagicTimerHandle);
		SetMagicChange(-20.0f);
		GetWorldTimerManager().SetTimer(MagicTimerHandle, this, &AFPSCharacter::ResetMagicToFull, 10.0f, false);

	}

}


void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}


void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSCharacter::SpawnBomb() 
{
	AFPSbombActor *MyBomb = GetWorld()->SpawnActor<AFPSbombActor>(BombClass, GetActorLocation(), GetActorRotation());
}

//-- get
float AFPSCharacter::GetHealth()
{
	return HealthPercentage;
}

float AFPSCharacter::GetMagic()
{
	return MagicPercentage;
}

//-- get Health
FText AFPSCharacter::GetHealthIntText() {

	int32 HP = FMath::RoundHalfFromZero(HealthPercentage * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString((TEXT("%")));
	FText HPText = FText::FromString(HealthHUD);

	return HPText;
}

//-- get Magic
FText AFPSCharacter::GetMagicIntText() {

	int32 MP = FMath::RoundHalfFromZero(MagicPercentage * 100);
	FString MPS = FString::FromInt(MP);
	FString FullMPS = FString::FromInt(FullMagic);
	FString MagicHUD = MPS + FString((TEXT("/"))) + FullMPS;
	FText MagicText = FText::FromString(MagicHUD);

	return MagicText;
}

//-- set
void AFPSCharacter::SetDamageState() {
	bCanBeDamaged = true;
}

void AFPSCharacter::DamageTimer() {
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AFPSCharacter::SetDamageState, 2.0f, false);
}

//-- set magic value
void AFPSCharacter::SetMagicValue() {
	TimelineValue = MyTimeline.GetPlaybackPosition();
	CurveFloatValue = PreviousMagic + MagicValue * MagicCurve->GetFloatValue(TimelineValue);
	Magic = FMath::Clamp(CurveFloatValue*FullHealth, 0.0f, FullMagic);
	MagicPercentage = FMath::Clamp(CurveFloatValue, 0.0f, 1.0f);
}

//-- set magic state
void AFPSCharacter::SetMagicState() {
	bCanUseMagic = true;
	UE_LOG(LogTemp, Warning, TEXT("Mat Change to default"));
	MagicValue = 0.0f;
	if (GunDefaultMaterial)
	{
		
		FP_Gun->SetMaterial(0, GunDefaultMaterial);
		//FP_Gun->SetMaterrial(0, GunDefaultMaterial);
	}
}

// flash
bool AFPSCharacter::PlayFlash() {
	if (redFlash) {	
		redFlash = false;
		return true;
	}
	return false;
}

void AFPSCharacter::ReceivePointDamage(float Damage,const UDamageType * DamageType,FVector HitLocation,FVector HitNormal,UPrimitiveComponent * HitComponent,FName BoneName,
	FVector ShotFromDirection,AController * InstigatedBy,AActor * DamageCauser,const FHitResult & HitInfo) {

	UE_LOG(LogTemp, Warning, TEXT("Receiving damage"));
	bCanBeDamaged = false;
	redFlash = true;

	UpdateHealth(-Damage);
	DamageTimer();

}

//////
float AFPSCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Receiving damage"));
	bCanBeDamaged = false;
	redFlash = true;
	UpdateHealth(-DamageAmount);
	DamageTimer();
	return DamageAmount;
}

//-- update health
void AFPSCharacter::UpdateHealth(float HealthChange) {

	Health += HealthChange;
	Health = FMath::Clamp(Health, 0.0f, FullHealth);
	HealthPercentage = Health / FullHealth;
}

//-- update magic to full
void  AFPSCharacter::ResetMagicToFull() {
	PreviousMagic = MagicPercentage;
	MagicPercentage = Magic / FullMagic;
	MagicValue = 1.0f;
	MyTimeline.PlayFromStart();
}

//-- Recharge Magic (from 3D Widget)
bool AFPSCharacter::RechargeMagic(float i_change) {
	if (MagicPercentage != 1)
	{
		MagicPercentage += i_change;
		Magic += i_change * 100;
		return true;
	}
	else
	{
		return false;
	}
}


//-- chnage magic
void AFPSCharacter::SetMagicChange(float MagicChnage) {
	
	bCanUseMagic = false;
	PreviousMagic = MagicPercentage;
	MagicValue = MagicChnage / FullMagic;
	if (GunOverheatMaterial)
	{
		FP_Gun->SetMaterial(0, GunOverheatMaterial);
	}

	MyTimeline.PlayFromStart();
}

