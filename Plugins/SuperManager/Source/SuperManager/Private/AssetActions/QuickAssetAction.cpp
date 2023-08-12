// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetActions/QuickAssetAction.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"

void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter a valid number"));
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		int32 RequiredDuplicates = NumOfDuplicates;
		uint32 CurrentIndexDuplicate = 1;

		while (RequiredDuplicates > 0)
		{
			if (CurrentIndexDuplicate <= 0)
			{
				DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Too many duplicates: " + FString::FromInt(RequiredDuplicates) + " haven't been instantiated"));
				break;
			}

			const FString SourceAssetPath = SelectedAssetData.ObjectPath.ToString();
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(CurrentIndexDuplicate);
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(), NewDuplicatedAssetName);

			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{
				UEditorAssetLibrary::SaveAsset(NewPathName, false);
				++Counter;
				--RequiredDuplicates;
			}

			++CurrentIndexDuplicate;
		}
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " files"));
	}
}

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;

	for (UObject* SelectedObject : SelectedObjects)
	{
		if (!SelectedObject)
		{
			continue;
		}

		FString* PrefixFound = PrefixMap.Find(SelectedObject->GetClass());
		if (!PrefixFound || PrefixFound->IsEmpty())
		{
			DebugHeader::Print(TEXT("Failed to find prefix for class " + SelectedObject->GetClass()->GetName()), FColor::Red);
			continue;
		}

		FString OldName = SelectedObject->GetName();
		if (OldName.StartsWith(*PrefixFound))
		{
			DebugHeader::Print(OldName + TEXT(" already has prefix added"), FColor::Red);
			continue;
		}

		if (SelectedObject->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromEnd(TEXT("_inst"));
			OldName.RemoveFromStart(TEXT("M_"));
		}

		const FString NewNameWithPrefix = *PrefixFound + OldName;
		
		UEditorUtilityLibrary::RenameAsset(SelectedObject, NewNameWithPrefix);

		++Counter;
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully renamed " + FString::FromInt(Counter) + " assets"));
	}
}

void UQuickAssetAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;

	FixUpRedirectors();

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.ObjectPath.ToString());
		if (AssetReferencers.Num() == 0)
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}

	if (UnusedAssetsData.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused assets found among selected assets"), false);
		return;
	}

	// Confirmation from user to delete assets
	int32 NumOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData, true);
	if (NumOfAssetsDeleted != 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully deleted " + FString::FromInt(NumOfAssetsDeleted) + " unused assets"));
	}
}

void UQuickAssetAction::FixUpRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFixArray;

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassNames.Emplace("ObjectRedirector");

	TArray<FAssetData> OutRedirectors;
	
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);

	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}
