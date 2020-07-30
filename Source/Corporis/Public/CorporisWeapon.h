// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Corporis.h"
#include "GameFramework/Actor.h"
#include "CorporisWeapon.generated.h"

UCLASS()
class CORPORIS_API ACorporisWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACorporisWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    virtual void PostInitializeComponents() override;

public:
    float GetAttackRange() { return AttackRange; }
    float GetAttackPower() { return AttackPower; }
    
private:
    UFUNCTION()
    void OnChampionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
    
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, Meta = (AllowPrivateAccess = true))
    USkeletalMeshComponent* Weapon;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status, Meta = (AllowPrivateAccess = true))
    float AttackRange;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status, Meta = (AllowPrivateAccess = true))
    float AttackPower;
};
