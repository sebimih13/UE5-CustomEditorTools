// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorActions/QuickActorActionsWidget.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "DebugHeader.h"

UQuickActorActionsWidget::UQuickActorActionsWidget()
	: SearchCase(ESearchCase::IgnoreCase)
	, AxisForDuplication(EDuplicationAxis::EDA_XAxis)
	, NumberOfDuplicates(5)
	, OffsetDist(300.0f)
	, EditorActorSubsystem(nullptr)
{

}

void UQuickActorActionsWidget::SelectAllActorsWithSimilarName()
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	TArray<AActor*> SelectedActorsArray = EditorActorSubsystem->GetSelectedLevelActors();

	if (SelectedActorsArray.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	if (SelectedActorsArray.Num() > 1)
	{
		DebugHeader::ShowNotifyInfo(TEXT("You can only select 1 actor"));
		return;
	}

	uint32 SelectedActorsCounter = 0;

	const FString SelectedActorName = SelectedActorsArray[0]->GetActorLabel();
	const FString NameToSearch = SelectedActorName.LeftChop(4);

	TArray<AActor*> LevelActorsArray = EditorActorSubsystem->GetAllLevelActors();
	for (AActor* ActorInLevel : LevelActorsArray)
	{
		if (!ActorInLevel)
		{
			continue;
		}

		if (ActorInLevel->GetActorLabel().Contains(NameToSearch, SearchCase))
		{
			EditorActorSubsystem->SetActorSelectionState(ActorInLevel, true);
			++SelectedActorsCounter;
		}
	}

	if (SelectedActorsCounter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully selected ") + FString::FromInt(SelectedActorsCounter) + TEXT(" actors"));
	}
	else
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor with similiar name found"));
	}
}

void UQuickActorActionsWidget::DuplicateActors()
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	TArray<AActor*> SelectedActorsArray = EditorActorSubsystem->GetSelectedLevelActors();

	if (SelectedActorsArray.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	if (NumberOfDuplicates <= 0 || OffsetDist == 0.0f)
	{
		return;
		DebugHeader::ShowNotifyInfo(TEXT("Specify number of duplication and offset distance"));
	}

	uint32 SelectedActorsCounter = 0;
	for (AActor* SelectedActor : SelectedActorsArray)
	{
		if (!SelectedActor)
		{
			continue;
		}

		for (int32 i = 0; i < NumberOfDuplicates; ++i)
		{
			AActor* DuplicatedActor = EditorActorSubsystem->DuplicateActor(SelectedActor, SelectedActor->GetWorld());
			if (!DuplicatedActor)
			{
				continue;
			}

			const float DuplicationOffsetDist = (i + 1) * OffsetDist;
			switch (AxisForDuplication)
			{
			case EDuplicationAxis::EDA_XAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(DuplicationOffsetDist, 0.0f, 0.0f));
				break;

			case EDuplicationAxis::EDA_YAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(0.0f, DuplicationOffsetDist, 0.0f));
				break;

			case EDuplicationAxis::EDA_ZAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(0.0f, 0.0f, DuplicationOffsetDist));
				break;
			}

			EditorActorSubsystem->SetActorSelectionState(DuplicatedActor, true);
			++SelectedActorsCounter;
		}
	}

	if (SelectedActorsCounter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully duplicated ") + FString::FromInt(SelectedActorsCounter) + TEXT(" actors"));
	}
}

void UQuickActorActionsWidget::RandomizeActorTransform()
{
	if (!RandomActorRotation.bRandomizeRotationYaw && 
		!RandomActorRotation.bRandomizeRotationPitch && 
		!RandomActorRotation.bRandomizeRotationRoll &&
		!RandomActorScale.bRandomizeScale &&
		!RandomActorOffset.bRandomizeOffset
		)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No variation condition specified"));
		return;
	}

	if (!GetEditorActorSubsystem())
	{
		return;
	}

	TArray<AActor*> SelectedActorsArray = EditorActorSubsystem->GetSelectedLevelActors();

	if (SelectedActorsArray.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	uint32 Counter = 0;
	for (AActor* SelectedActor : SelectedActorsArray)
	{
		if (!SelectedActor)
		{
			continue;
		}

		if (RandomActorRotation.bRandomizeRotationYaw)
		{
			const float RandomRotationYawValue = FMath::RandRange(RandomActorRotation.RotationYawMin, RandomActorRotation.RotationYawMax);
			SelectedActor->AddActorWorldRotation(FRotator(0.0f, RandomRotationYawValue, 0.0f));
		}

		if (RandomActorRotation.bRandomizeRotationPitch)
		{
			const float RandomRotationPitchValue = FMath::RandRange(RandomActorRotation.RotationPitchMin, RandomActorRotation.RotationPitchMax);
			SelectedActor->AddActorWorldRotation(FRotator(RandomRotationPitchValue, 0.0f, 0.0f));
		}

		if (RandomActorRotation.bRandomizeRotationRoll)
		{
			const float RandomRotationRollValue = FMath::RandRange(RandomActorRotation.RotationRollMin, RandomActorRotation.RotationRollMax);
			SelectedActor->AddActorWorldRotation(FRotator(RandomRotationRollValue, 0.0f, 0.0f));
		}

		if (RandomActorScale.bRandomizeScale)
		{
			const float RandomScaleValue = FMath::RandRange(RandomActorScale.ScaleMin, RandomActorScale.ScaleMax);
			SelectedActor->SetActorScale3D(FVector(RandomScaleValue));
		}

		if (RandomActorOffset.bRandomizeOffset)
		{
			const float RandomOffsetValue = FMath::RandRange(RandomActorOffset.OffsetMin, RandomActorOffset.OffsetMax);
			SelectedActor->AddActorWorldOffset(FVector(RandomOffsetValue, RandomOffsetValue, 0.0f));
		}
		
		++Counter;
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully set ") + FString::FromInt(Counter) + TEXT(" actors"));
	}
}

bool UQuickActorActionsWidget::GetEditorActorSubsystem()
{
	if (!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}

	return EditorActorSubsystem != nullptr;
}
