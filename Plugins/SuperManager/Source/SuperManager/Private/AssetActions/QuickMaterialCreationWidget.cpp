// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetActions/QuickMaterialCreationWidget.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialFactoryNew.h"

UQuickMaterialCreationWidget::UQuickMaterialCreationWidget()
	: bCustomMaterialName(true)
	, MaterialName(TEXT("M_"))
{
	// Default values for Base Color Array
	BaseColorArray.Add(TEXT("_BaseColor"));
	BaseColorArray.Add(TEXT("_Albedo"));
	BaseColorArray.Add(TEXT("_Diffuse"));
	BaseColorArray.Add(TEXT("_diff"));

	// Default values for Metallic Array
	MetallicArray.Add(TEXT("_Metallic"));
	MetallicArray.Add(TEXT("_metal"));

	// Default values for Roughness Array
	RoughnessArray.Add(TEXT("_Roughness"));
	RoughnessArray.Add(TEXT("_RoughnessMap"));
	RoughnessArray.Add(TEXT("_rough"));

	// Default values for Normal Array
	NormalArray.Add(TEXT("_NormalMap"));
	NormalArray.Add(TEXT("_nor"));
	NormalArray.Add(TEXT("_Normal"));

	// Default values for Ambient Occlusion Array
	AmbientOcclusionArray.Add(TEXT("_AmbientOcclusion"));
	AmbientOcclusionArray.Add(TEXT("_AmbientOcclusionMap"));
	AmbientOcclusionArray.Add(TEXT("_AO"));
}

void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	if (bCustomMaterialName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter a valid name"));
			return;
		}
	}

	TArray<FAssetData> SelectedAssetsDataArray = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UTexture2D*> SelectedTexturesArray;
	FString SelectedTextureFolderPath;

	if (!ProcessSelectedData(SelectedAssetsDataArray, SelectedTexturesArray, SelectedTextureFolderPath))
	{
		return;
	}

	if (CheckIsNameUsed(SelectedTextureFolderPath, MaterialName))
	{
		return;
	}

	UMaterial* CreatedMaterial = CreateMaterialAsset(MaterialName, SelectedTextureFolderPath);
	if (!CreatedMaterial)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Failed to create material"));
		return;
	}
}

bool UQuickMaterialCreationWidget::ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProcessArray, TArray<UTexture2D*>& OutSelectedTexturesArray, FString& OutSelectedTexturePackagePath)
{
	if (SelectedDataToProcessArray.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No texture selected"));
		return false;
	}

	bool bIsMaterialNameSet = false;
	for (const FAssetData& SelectedData : SelectedDataToProcessArray)
	{
		UObject* SelectedAsset = SelectedData.GetAsset();
		if (!SelectedAsset)
		{
			continue;
		}

		UTexture2D* SelectedTexture = Cast<UTexture2D>(SelectedAsset);
		if (!SelectedTexture)
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please select only textures\n") + SelectedAsset->GetName() + TEXT(" is not a texture"));
			return false;
		}

		OutSelectedTexturesArray.Add(SelectedTexture);

		if (OutSelectedTexturePackagePath.IsEmpty())
		{
			OutSelectedTexturePackagePath = SelectedData.PackagePath.ToString();
		}

		if (!bCustomMaterialName && !bIsMaterialNameSet)
		{
			MaterialName = SelectedAsset->GetName();
			MaterialName.RemoveFromStart(TEXT("T_"));
			MaterialName.InsertAt(0, TEXT("M_"));

			bIsMaterialNameSet = true;
		}
	}

	return true;
}

bool UQuickMaterialCreationWidget::CheckIsNameUsed(const FString& FolderPathToCheck, const FString& MaterialNameToCheck)
{
	TArray<FString> ExistingAssetsPathsArray = UEditorAssetLibrary::ListAssets(FolderPathToCheck, false);
	for (const FString& ExistingAssetPath : ExistingAssetsPathsArray)
	{
		const FString ExistingAssetName = FPaths::GetBaseFilename(ExistingAssetPath);

		if (ExistingAssetName.Equals(MaterialNameToCheck))
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok, MaterialNameToCheck + TEXT(" is already used"));
			return true;
		}
	}

	return false;
}

UMaterial* UQuickMaterialCreationWidget::CreateMaterialAsset(const FString& NewMaterialAssetName, const FString& MaterialPath)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	UObject* CreatedObject = AssetToolsModule.Get().CreateAsset(NewMaterialAssetName, MaterialPath, UMaterial::StaticClass(), NewObject<UMaterialFactoryNew>());

	return Cast<UMaterial>(CreatedObject);
}
