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
	, ChannelPackingType(EChannelPackingType::ECPT_NoChannelPacking)
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

	// Default values for ORM
	ORMArray.Add(TEXT("_arm"));
	ORMArray.Add(TEXT("_OcclusionRoughnessMetallic"));
	ORMArray.Add(TEXT("_ORM"));
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

	uint32 PinsConnectedCounter = 0;
	for (UTexture2D* SelectedTexture : SelectedTexturesArray)
	{
		if (!SelectedTexture)
		{
			continue;
		}

		switch (ChannelPackingType)
		{
		case EChannelPackingType::ECPT_NoChannelPacking:
			DefaultCreateMaterialNodes(CreatedMaterial, SelectedTexture, PinsConnectedCounter);
			break;

		case EChannelPackingType::ECPT_ORM:
			ORMCreateMaterialNodes(CreatedMaterial, SelectedTexture, PinsConnectedCounter);
			break;

		case EChannelPackingType::ECPT_MAX:
			break;

		default:
			break;
		}

	}

	if (PinsConnectedCounter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully connected ") + FString::FromInt(PinsConnectedCounter) + TEXT(" pins"));
	}

	// Reset MaterialName
	MaterialName = TEXT("M_");
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

void UQuickMaterialCreationWidget::DefaultCreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);
	if (!TextureSampleNode)
	{
		return;
	}

	if (!CreatedMaterial->BaseColor.IsConnected())
	{
		if (TryConnectBaseColor(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			++PinsConnectedCounter;
			return;
		}
	}

	if (!CreatedMaterial->Metallic.IsConnected())
	{
		if (TryConnectMetallic(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			++PinsConnectedCounter;
			return;
		}
	}

	if (!CreatedMaterial->Roughness.IsConnected())
	{
		if (TryConnectRoughness(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			++PinsConnectedCounter;
			return;
		}
	}

	if (!CreatedMaterial->Normal.IsConnected())
	{
		if (TryConnectNormal(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			++PinsConnectedCounter;
			return;
		}
	}

	if (!CreatedMaterial->AmbientOcclusion.IsConnected())
	{
		if (TryConnectAmbientOcclusion(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			++PinsConnectedCounter;
			return;
		}
	}

	DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Failed to connect the texture: ") + SelectedTexture->GetName());
}

void UQuickMaterialCreationWidget::ORMCreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);
	if (!TextureSampleNode)
	{
		return;
	}

	if (!CreatedMaterial->BaseColor.IsConnected())
	{
		if (TryConnectBaseColor(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			++PinsConnectedCounter;
			return;
		}
	}

	if (!CreatedMaterial->Normal.IsConnected())
	{
		if (TryConnectNormal(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			++PinsConnectedCounter;
			return;
		}
	}

	if (!CreatedMaterial->AmbientOcclusion.IsConnected() && !CreatedMaterial->Roughness.IsConnected() && !CreatedMaterial->Metallic.IsConnected())
	{
		if (TryConnectORM(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter += 3;
			return;
		}
	}

	DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Failed to connect the texture: ") + SelectedTexture->GetName());
}

bool UQuickMaterialCreationWidget::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& BaseColorName : BaseColorArray)
	{
		if (SelectedTexture->GetName().Contains(BaseColorName))
		{
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->MaterialExpressionEditorX -= 600;

			CreatedMaterial->Expressions.Add(TextureSampleNode);
			CreatedMaterial->BaseColor.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectMetallic(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& MetallicName : MetallicArray)
	{
		if (SelectedTexture->GetName().Contains(MetallicName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();
			
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;
			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 240;

			CreatedMaterial->Expressions.Add(TextureSampleNode);
			CreatedMaterial->Metallic.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& RoughnessName : RoughnessArray)
	{
		if (SelectedTexture->GetName().Contains(RoughnessName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;
			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 480;

			CreatedMaterial->Expressions.Add(TextureSampleNode);
			CreatedMaterial->Roughness.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& NormalName : NormalArray)
	{
		if (SelectedTexture->GetName().Contains(NormalName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Normalmap;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_Normal;
			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 720;

			CreatedMaterial->Expressions.Add(TextureSampleNode);
			CreatedMaterial->Normal.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectAmbientOcclusion(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& AmbientOcclusionName : AmbientOcclusionArray)
	{
		if (SelectedTexture->GetName().Contains(AmbientOcclusionName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;
			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 960;

			CreatedMaterial->Expressions.Add(TextureSampleNode);
			CreatedMaterial->AmbientOcclusion.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& ORMName : ORMArray)
	{
		if (SelectedTexture->GetName().Contains(ORMName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Masks;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_Masks;
			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 960;

			CreatedMaterial->Expressions.Add(TextureSampleNode);
			CreatedMaterial->AmbientOcclusion.Connect(1, TextureSampleNode);
			CreatedMaterial->Roughness.Connect(2, TextureSampleNode);
			CreatedMaterial->Metallic.Connect(3, TextureSampleNode);
			CreatedMaterial->PostEditChange();

			return true;
		}
	}

	return false;
}
