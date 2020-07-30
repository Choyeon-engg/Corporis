// Fill out your copyright notice in the Description page of Project Settings.


#include "CorporisChampion.h"
#include "CorporisMinion.h"
#include "CorporisWeapon.h"
#include "Components/PawnNoiseEmitterComponent.h"

// Sets default values
ACorporisChampion::ACorporisChampion() : ChampionHP(8000), LastFootstep(0.0f), DeadTimer(0.03f)
{
     // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    GetMesh()->SetCollisionProfileName(TEXT("CorporisActor"));
    
    NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Noise Emitter"));
}

// Called when the game starts or when spawned
void ACorporisChampion::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void ACorporisChampion::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector VelocityVector = GetCharacterMovement()->Velocity;
    float VectorMagnitude = VelocityVector.Size();

    float Now = GetWorld()->GetTimeSeconds();

    if (Now > LastFootstep + 1.0f && VectorMagnitude > 0.0f && !GetCharacterMovement()->IsCrouching())
    {
        MakeNoise(0.5f, this, FVector::ZeroVector);
        LastFootstep = Now;
    }
}

void ACorporisChampion::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    CorporisAnim = Cast<UCorporisAnimInstance>(GetMesh()->GetAnimInstance());
    
    CorporisAnim->CEnablePhysics.AddLambda([this]() -> void {
        GetMesh()->SetSimulatePhysics(true);
        
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void {
            Destroy();
        }), DeadTimer, false);
    });
}

// Called to bind functionality to input
void ACorporisChampion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ACorporisChampion::Attack);
    PlayerInputComponent->BindAction(TEXT("Sit"), EInputEvent::IE_Pressed, this, &ACorporisChampion::Sit);
    PlayerInputComponent->BindAction(TEXT("NotSit"), EInputEvent::IE_Released, this, &ACorporisChampion::Stand);
}

float ACorporisChampion::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    FHitResult HitResult;
    FVector ImpulseDirection;
    DamageEvent.GetBestHitInfo(this, DamageCauser, HitResult, ImpulseDirection);
    
    if (HitResult.BoneName == "Head")
    {
        ChampionHP = 0;
        OnHPChanged.Broadcast();
        
        CorporisAnim->SetIsDead(true);
    }
    
    else
    {
        ChampionHP -= DamageAmount;
        OnHPChanged.Broadcast();
        
        CorporisAnim->SetIsDamaged(true);
        
        if (ChampionHP <= 0)
            CorporisAnim->SetIsDead(true);
    }
    
    OnHPChanged.Broadcast();
    
    return FinalDamage;
}

void ACorporisChampion::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    
    GetCharacterMovement()->MaxWalkSpeed = 500.0f;
}

void ACorporisChampion::SetWeapon(ACorporisWeapon* NewWeapon)
{
    FName WeaponSocket(TEXT("hand_r"));
    
    if (CurrentWeapon)
        CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
    
    if (NewWeapon)
    {
        NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
        NewWeapon->SetOwner(this);
        CurrentWeapon = NewWeapon;
    }
}

bool ACorporisChampion::ChampionIsDead() const
{
    if (ChampionHP <= 0) { return true; }
    
    return false;
}

float ACorporisChampion::GetHPRatio()
{
    return ((ChampionHP < KINDA_SMALL_NUMBER) ? 0.0f : (ChampionHP / 8000.0f));
}

void ACorporisChampion::Attack()
{
    FHitResult HitResult;
    FCollisionQueryParams Params(NAME_None, false, this);
    bool bResult = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * (CurrentWeapon ? CurrentWeapon->GetAttackRange() : 3000.0f), ECollisionChannel::ECC_GameTraceChannel1, Params);
    auto Target = Cast<ACorporisMinion>(HitResult.Actor);
    
    MakeNoise(1.0, this, FVector::ZeroVector);
    
    if (bResult && Target)
        UGameplayStatics::ApplyPointDamage(Target, (CurrentWeapon ? CurrentWeapon->GetAttackPower() : 80.0f), GetActorForwardVector(), HitResult, GetController(), this, nullptr);
}
