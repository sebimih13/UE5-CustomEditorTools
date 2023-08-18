// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActionsWidget.generated.h"

/** Forward Declarations */
class UEditorActorSubsystem;

UENUM(BlueprintType)
enum class EDuplicationAxis : uint8
{
	EDA_XAxis		UMETA(DisplayName = "X Axis"),
	EDA_YAxis		UMETA(DisplayName = "Y Axis"),
	EDA_ZAxis		UMETA(DisplayName = "Z Axis"),
	EDA_DefaultMAX	UMETA(DisplayName = "Default MAX")
};

USTRUCT(BlueprintType)
struct FRandomActorRotation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorRotation")
	bool bRandomizeRotationYaw = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorRotation", meta = (EditCondition = "bRandomizeRotationYaw"))
	float RotationYawMin = -45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorRotation", meta = (EditCondition = "bRandomizeRotationYaw"))
	float RotationYawMax = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorRotation")
	bool bRandomizeRotationPitch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorRotation", meta = (EditCondition = "bRandomizeRotationPitch"))
	float RotationPitchMin = -45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorRotation", meta = (EditCondition = "bRandomizeRotationPitch"))
	float RotationPitchMax = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorRotation")
	bool bRandomizeRotationRoll = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorRotation", meta = (EditCondition = "bRandomizeRotationRoll"))
	float RotationRollMin = -45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorRotation", meta = (EditCondition = "bRandomizeRotationRoll"))
	float RotationRollMax = 45.0f;
};

USTRUCT(BlueprintType)
struct FRandomActorScale
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorScale")
	bool bRandomizeScale = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorScale", meta = (EditCondition = "bRandomizeScale"))
	float ScaleMin = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorScale", meta = (EditCondition = "bRandomizeScale"))
	float ScaleMax = 0.8f;
};

USTRUCT(BlueprintType)
struct FRandomActorOffset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorOffset")
	bool bRandomizeOffset = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorOffset", meta = (EditCondition = "bRandomizeOffset"))
	float OffsetMin = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomActorOffset", meta = (EditCondition = "bRandomizeOffset"))
	float OffsetMax = 50.0f;
};

/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickActorActionsWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	/** Constructor */
	UQuickActorActionsWidget();

	/** Selection */
	UFUNCTION(BlueprintCallable, Category = "ActorBatchSelection")
	void SelectAllActorsWithSimilarName();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type> SearchCase;

	/** Duplication */
	UFUNCTION(BlueprintCallable, Category = "ActorBatchDuplication")
	void DuplicateActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	EDuplicationAxis AxisForDuplication;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	int32 NumberOfDuplicates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	float OffsetDist;

	/** Duplication */
	UFUNCTION(BlueprintCallable, Category = "RandomizeActorTransform")
	void RandomizeActorTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomizeActorTransform")
	FRandomActorRotation RandomActorRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomizeActorTransform")
	FRandomActorScale RandomActorScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomizeActorTransform")
	FRandomActorOffset RandomActorOffset;
	
private:
	UPROPERTY()
	UEditorActorSubsystem* EditorActorSubsystem;

	bool GetEditorActorSubsystem();
};
