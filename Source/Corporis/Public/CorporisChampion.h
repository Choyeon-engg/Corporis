// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Corporis.h"
#include "GameFramework/Character.h"
#include "CorporisAnimInstance.h"
#include "CorporisChampion.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPChangedDelegate);

UCLASS()
class CORPORIS_API ACorporisChampion : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ACorporisChampion();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void PostInitializeComponents() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    
    virtual void PossessedBy(AController* NewController) override;
    
    bool ChampionIsDead() const;
    float GetHPRatio();
    
    FOnHPChangedDelegate OnHPChanged;
    
private:
    void Attack();
    void Sit() { CorporisAnim->SetIsSit(true); }
    void Stand() { CorporisAnim->SetIsSit(false); }

private:
    int32 ChampionHP;
    float LastFootstep;
    float DeadTimer;
    
    FTimerHandle DeadTimerHandle = { };
    
    UPROPERTY()
    UCorporisAnimInstance* CorporisAnim;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Noise, Meta = (AllowPrivateAccess = true))
    class UPawnNoiseEmitterComponent* NoiseEmitter;
};
