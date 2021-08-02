// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "MyPawn.h"
#include <cmath>

struct limit_t {
	float len;
	FVector base;
};

struct verlet_t {
	FVector prev_r;
	FVector v;
	void init(const FVector& inipos)
	{
		prev_r = inipos;
		//inv_a = 0.f;
		v = FVector();
	}
	
	FVector extrapolation(FVector&& d, const FVector& r, float dt, TOptional<limit_t>&& limit, bool dumper = true)
	{
		auto m = 40.f;
		float dt2 = dt * dt;
		auto a = d / dt;
		if (dumper)
			a -= (v.GetSafeNormal() * v.Size() * v.Size()) / dt; /* ダンパー */
		auto next_r = 2.f * r - prev_r + a * dt2;
		if (limit) {
			auto&& l = limit.GetValue();
			if ((next_r - l.base).Size() > l.len)  {
				next_r = r;
			}
		}
		v = (next_r - r);
		prev_r = r;
		return next_r;
	}
	// helper
	FVector prev_loc;
};
verlet_t verlet;


AMyPlayerController::AMyPlayerController() : APlayerController(), Stopping(false), NoAccel(false), UpdateDiff(true), Direction(false), DirectionChanged(false), Frames(0)
{
	PrimaryActorTick.bCanEverTick = true;
}

AMyPlayerController::~AMyPlayerController()
{
}

void AMyPlayerController::Tick(float delta)
{
	Super::Tick(delta);
	//UE_LOG(LogTemp, Warning, TEXT("Tick"));

	if (Target && Dst) {
		FVector loc = Target->GetActorLocation();
		FVector curpos = Dst->GetActorLocation();
		Neutral = FVector(loc.X, curpos.Y, curpos.Z);
		if (!UpdateDiff) {
			Frames = std::ceil(1.f / delta);
			Diff = (Neutral - curpos) / Frames;
			UpdateDiff = true;
			verlet.init(curpos);
			UE_LOG(LogTemp, Error, TEXT("UpdateDiff: Neutral:%s curpos:%s"), *Neutral.ToString(), *curpos.ToString()); 
		}

		FVector r;
		if (Stopping) {
			FVector d = Diff;
			//UE_LOG(LogTemp, Warning, TEXT("Diff:%s Frames:%d"), *Diff.ToString(), Frames); 
			r = verlet.extrapolation(std::move(d), curpos, delta, TOptional< limit_t >(), false);
			Frames --;
			if ((Neutral - r).Size() < 0.1f || Frames <= 0) {
				Diff = FVector();
				Stopping = false;
				loc = Neutral;
				verlet.init(Neutral);
				r = Neutral;
			}
			NoAccel = false;
		}
		else {
			if (NoAccel) {
				verlet.extrapolation((loc - verlet.prev_loc) * 4.f, curpos, delta, TOptional<limit_t>(), false);
				r = curpos + (loc - verlet.prev_loc);
			}
			else {
				r = verlet.extrapolation((loc - verlet.prev_loc) * 4.f, curpos, delta, TOptional<limit_t>(/* limit_t{100.f, Neutral} */), false);
#if 1
				if (!DirectionChanged && (r - Neutral).Size() > 100.f) {
					NoAccel = true;
				}
				else if (DirectionChanged) {
					NoAccel = false;
					Stopping = false;
					/* 張り付き防止: 方向チェンジ直後は次の tick で NoAccel=true になってしまうことがあるので
					    一定まで下がるまでは DirectionChanged を解除しない.
					 */
					if ((r - Neutral).Size() < 50.f) 
						DirectionChanged = false;
				}
#endif
			}
		}
		Dst->SetActorLocation(FVector(r.X, curpos.Y, curpos.Z), false, nullptr, ETeleportType::TeleportPhysics);
		verlet.prev_loc  = loc;
	}
}


void AMyPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	auto pawn = GetPawnOrSpectator();
	auto w = GetWorld();
	{
		TArray<AActor*> a;
		UGameplayStatics::GetAllActorsOfClass(w, AActor::StaticClass(),  a);
		for (auto&& i : a) {
			if (i->ActorHasTag(FName("dst")))
				Dst = i;
		}
	}

#if 1
	if (!pawn) {
		TArray<AActor*> b;
		UGameplayStatics::GetAllActorsOfClass(w, APawn::StaticClass(),  b);
		if (b.Num() > 0)
			Target = b[0];
	}
	else {
		Target = pawn;
	}
#endif

	verlet.prev_loc = Target->GetActorLocation();
	verlet.init(Dst->GetActorLocation());
}
