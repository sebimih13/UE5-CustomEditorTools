// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetActions/QuickAssetAction.h"
#include "DebugHeader.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter a valid number"));
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
				ShowMsgDialog(EAppMsgType::Ok, TEXT("Too many duplicates: " + FString::FromInt(RequiredDuplicates) + " haven't been instantiated"));
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
		ShowNotifyInfo(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " files"));
	}
}
