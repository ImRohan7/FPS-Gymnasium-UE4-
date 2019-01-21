// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "FPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class AFPSProjectile;
class USoundBase;
class UAnimSequence;
class AFPSbombActor;

UCLASS()
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	/** Pawn mesh: 1st person view  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	USkeletalMeshComponent* Mesh1PComponent;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* FP_Gun;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	void SpawnBomb();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime) override;

public:
	AFPSCharacter();

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	TSubclassOf<AFPSProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	UAnimSequence* FireAnimation;

	// Bomb that spawns
	UPROPERTY(EditDefaultsOnly, category = "Bombs")
	TSubclassOf<AFPSbombActor> BombClass;

// Player Variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float FullHealth;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float Magic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float FullMagic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float MagicPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float PreviousMagic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	float MagicValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool redFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
	UCurveFloat * MagicCurve;

	UPROPERTY(EditAnywhere, Category = "Magic")
	FTimeline MyTimeline;

	UPROPERTY(EditAnywhere, Category = "Magic")
	FTimerHandle MemberTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Magic")
	FTimerHandle MagicTimerHandle;

	float CurveFloatValue;
	float TimelineValue;
	bool bCanUseMagic;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
	FText GetHealthIntText();


	UFUNCTION(BlueprintPure, Category = "Magic")
	float GetMagic();

	UFUNCTION(BlueprintCallable, Category = "Magic")
		bool RechargeMagic(float change);

	UFUNCTION(BlueprintPure, Category = "Magic")
	FText GetMagicIntText();

	//UFUNCTION(BlueprintPure, Category = "Magic")
	void DamageTimer();

	UFUNCTION()
	void SetDamageState();

	UFUNCTION()
		void SetMagicValue();

	UFUNCTION()
		void SetMagicState();

	UFUNCTION()
		void SetMagicChange(float MagicChange);

	UFUNCTION()
		void ResetMagicToFull();

	UFUNCTION(BlueprintPure, Category = "Health")
		bool PlayFlash();

	UPROPERTY(EditAnywhere, Category = "Magic")
		class UMaterialInterface* GunDefaultMaterial;

	UPROPERTY(EditAnywhere, Category = "Magic")
		class UMaterialInterface* GunOverheatMaterial;

	//UFUNCTION()
		void ReceivePointDamage
		(
			float Damage,
			const UDamageType * DamageType,
			FVector HitLocation,
			FVector HitNormal,
			UPrimitiveComponent * HitComponent,
			FName BoneName,
			FVector ShotFromDirection,
			AController * InstigatedBy,
			AActor * DamageCauser,
			const FHitResult & HitInfo
		);

	UFUNCTION(BlueprintCallable, Category = "Health")
		void UpdateHealth(float HealthChange);

protected:
	
	/** Fires a projectile. */
	void Fire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1PComponent; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return CameraComponent; }

};

