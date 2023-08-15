// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickMaterialCreationWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickMaterialCreationWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
	UQuickMaterialCreationWidget();

	UFUNCTION(BlueprintCallable)
	void CreateMaterialFromSelectedTextures();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CreateMaterialFromSelectedTextures")
	bool bCustomMaterialName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CreateMaterialFromSelectedTextures", meta = (EditCondition = "bCustomMaterialName"))
	FString MaterialName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SupportedTextureNames")
	TArray<FString> BaseColorArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SupportedTextureNames")
	TArray<FString> MetallicArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SupportedTextureNames")
	TArray<FString> RoughnessArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SupportedTextureNames")
	TArray<FString> NormalArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SupportedTextureNames")
	TArray<FString> AmbientOcclusionArray;

private:
	bool ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProcessArray, TArray<UTexture2D*>& OutSelectedTexturesArray, FString& OutSelectedTexturePackagePath);
	bool CheckIsNameUsed(const FString& FolderPathToCheck, const FString& MaterialNameToCheck);
	UMaterial* CreateMaterialAsset(const FString& NewMaterialAssetName, const FString& MaterialPath);
};
