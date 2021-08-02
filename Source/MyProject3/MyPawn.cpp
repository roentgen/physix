// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyPawn::AMyPawn(const FObjectInitializer& o)// : APawn(o)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

#if 0
void AMyPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//auto pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//pc->SetPawn(this);
}
#endif

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	FInputActionBinding l("Left", IE_Repeat);
	FInputActionBinding r("Right", IE_Repeat);
	l.ActionDelegate.GetDelegateForManualSet().BindLambda( [this]() {
		//UE_LOG(LogTemp, Error, TEXT("Left"));
		auto v = GetActorLocation();
		SetActorLocation(v + FVector(10.f, 0.f, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
	});
	r.ActionDelegate.GetDelegateForManualSet().BindLambda( [this]() {
		//UE_LOG(LogTemp, Error, TEXT("Right")); 
		auto v = GetActorLocation();
		SetActorLocation(v + FVector(-10.f, 0.f, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
	});

	FInputActionBinding l_p("Left", IE_Pressed);
	FInputActionBinding l_r("Left", IE_Released);
	FInputActionBinding r_p("Right", IE_Pressed);
	FInputActionBinding r_r("Right", IE_Released);

	auto pc = Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	l_p.ActionDelegate.GetDelegateForManualSet().BindLambda([=]() { UE_LOG(LogTemp, Error, TEXT("Left Pressed")); pc->Stop(false); pc->ChangeDirection(false); });
	l_r.ActionDelegate.GetDelegateForManualSet().BindLambda([=]() { UE_LOG(LogTemp, Error, TEXT("Left Released")); pc->Stop(true); });
	r_p.ActionDelegate.GetDelegateForManualSet().BindLambda([=]() { UE_LOG(LogTemp, Error, TEXT("Right Pressed"));  pc->Stop(false); pc->ChangeDirection(true); });
	r_r.ActionDelegate.GetDelegateForManualSet().BindLambda([=]() { UE_LOG(LogTemp, Error, TEXT("Right Released")); pc->Stop(true); });
	
	PlayerInputComponent->AddActionBinding(l);
	PlayerInputComponent->AddActionBinding(l_p);
	PlayerInputComponent->AddActionBinding(l_r);
	PlayerInputComponent->AddActionBinding(r);
	PlayerInputComponent->AddActionBinding(r_p);
	PlayerInputComponent->AddActionBinding(r_r);
}


void AMyPawn::LeftPressed()
{
	UE_LOG(LogTemp, Error, TEXT("Left in"));
}

void AMyPawn::LeftReleased()
{
	UE_LOG(LogTemp, Error, TEXT("Left out"));
}

void AMyPawn::RightPressed()
{
	UE_LOG(LogTemp, Error, TEXT("Right in"));
}

void AMyPawn::RightReleased()
{
	UE_LOG(LogTemp, Error, TEXT("Right out"));
}


