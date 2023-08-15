// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickMaterialCreationWidget.generated.h"

/** Forward Declarations */
class UMaterialInstanceConstant;

UENUM(BlueprintType)
enum class EChannelPackingType : uint8
{
	ECPT_NoChannelPacking	UMETA(DisplayName = "No Channel Packing"),
	ECPT_ORM				UMETA(DisplayName = "Occlusion-Roughness-Metallic"),
	ECPT_MAX				UMETA(DisplayName = "DefaultMAX")
};

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
	EChannelPackingType ChannelPackingType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CreateMaterialFromSelectedTextures")
	bool bCreateMaterialInstance;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SupportedTextureNames")
	TArray<FString> ORMArray;

private:
	bool ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProcessArray, TArray<UTexture2D*>& OutSelectedTexturesArray, FString& OutSelectedTexturePackagePath);
	bool CheckIsNameUsed(const FString& FolderPathToCheck, const FString& MaterialNameToCheck);
	UMaterial* CreateMaterialAsset(const FString& NewMaterialAssetName, const FString& MaterialPath);
	UMaterialInstanceConstant* CreateMaterialInstanceAsset(UMaterial* MaterialParent, const FString& MaterialInstancePath);

	void DefaultCreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnectedCounter);
	void ORMCreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnectedCounter);

	/** Connect the required pins */
	bool TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	bool TryConnectMetallic(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	bool TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	bool TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	bool TryConnectAmbientOcclusion(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
	bool TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial);
};

// TODO : const la pointers