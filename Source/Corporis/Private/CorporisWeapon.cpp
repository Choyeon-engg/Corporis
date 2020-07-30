// Fill out your copyright notice in the Description page of Project Settings.


#include "CorporisWeapon.h"
#include "CorporisChampion.h"

// Sets default values
ACorporisWeapon::ACorporisWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
    RootComponent = Weapon;
    
    Weapon->SetCollisionProfileName(TEXT("CorporisWeapon"));
    Weapon->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ACorporisWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACorporisWeapon::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    Weapon->OnComponentBeginOverlap.AddDynamic(this, &ACorporisWeapon::OnChampionOverlap);
}

void ACorporisWeapon::OnChampionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    auto CorporisChampion = Cast<ACorporisChampion>(OtherActor);
    
    if (CorporisChampion)
        CorporisChampion->SetWeapon(this);
}
