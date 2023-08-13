// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManagerModule.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	InitContentBrowserMenuExtension();
	RegisterAdvancedDeletionTab();
}

void FSuperManagerModule::ShutdownModule()
{

}

#pragma region ContentBrowserMenuExtension

void FSuperManagerModule::InitContentBrowserMenuExtension()
{
	// Get all menu extenders
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModuleMenuExtenders = ContentBrowserModule.GetAllPathViewContextMenuExtenders();

	// Add a new custom delegate
	ContentBrowserModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FSuperManagerModule::CustomContentBrowserMenuExtender));
}

TSharedRef<FExtender> FSuperManagerModule::CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		MenuExtender->AddMenuExtension(TEXT("Delete"), EExtensionHook::After, TSharedPtr<FUICommandList>(), FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddContentBrowserMenuEntry));

		FoldersPathSelectedArray = SelectedPaths;
	}

	return MenuExtender;
}

void FSuperManagerModule::AddContentBrowserMenuEntry(FMenuBuilder& MenuBuilder)
{
	// Delete unused assets
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Delete unused assets")),
		FText::FromString(TEXT("Safely delete all unused assets under folder")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetsButtonClicked)
	);

	// Delete empty folders
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete empty folders")),
		FText::FromString(TEXT("Safely delete all empty folders and subfolders")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked)
	);

	// Advanced Deletion
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Advanced Deletion")),
		FText::FromString(TEXT("List assets by specific conditions in a tab for deleting")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnAdvancedDeletionButtonClicked)
	);
}

void FSuperManagerModule::OnDeleteUnusedAssetsButtonClicked()
{
	if (FoldersPathSelectedArray.Num() > 1)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("You can only do this to one folder"));
		return;
	}

	TArray<FString> AssetsPathNameArray = UEditorAssetLibrary::ListAssets(FoldersPathSelectedArray[0]);
	if (AssetsPathNameArray.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No asset found under selected folder"));
		return;
	}

	EAppReturnType::Type ConfirmResult = DebugHeader::ShowMsgDialog(EAppMsgType::YesNo, TEXT("A total of ") + FString::FromInt(AssetsPathNameArray.Num()) + TEXT(" assets found.\nWould you like to procceed?"));
	if (ConfirmResult == EAppReturnType::No)
	{
		return;
	}

	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetsDataArray;
	for (const FString& AssetPathName : AssetsPathNameArray)
	{
		// Don't touch the root folder
		if (AssetPathName.Contains(TEXT("Developers")) || AssetPathName.Contains(TEXT("Collections")) 
			|| AssetPathName.Contains(TEXT("__ExternalActors__")) || AssetPathName.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		// Verify if the asset exists
		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		TArray<FString> AssetReferencesArray = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		if (AssetReferencesArray.Num() == 0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsDataArray.Add(UnusedAssetData);
		}
	}

	if (UnusedAssetsDataArray.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsDataArray);

		EAppReturnType::Type DeleteEmptyFoldersResult = DebugHeader::ShowMsgDialog(EAppMsgType::YesNo, TEXT("Would you like to delete the empty folders?"), false);
		if (ConfirmResult == EAppReturnType::Yes)
		{
			OnDeleteEmptyFoldersButtonClicked();
		}
	}
	else
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused asset found under selected folder"));
	}
}

void FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked()
{
	FixUpRedirectors();

	FString EmptyFoldersPathsNames;
	TArray<FString> EmptyFoldersPathsArray;

	// First check the subfolders
	for (const FString& FolderPathSelected : FoldersPathSelectedArray)
	{
		TArray<FString> SubfoldersPathArray = UEditorAssetLibrary::ListAssets(FolderPathSelected, true, true);
		for (const FString& FolderPath : SubfoldersPathArray)
		{
			// Don't touch the root folder
			if (FolderPath.Contains(TEXT("Developers")) || FolderPath.Contains(TEXT("Collections"))
				|| FolderPath.Contains(TEXT("__ExternalActors__")) || FolderPath.Contains(TEXT("__ExternalObjects__")))
			{
				continue;
			}

			if (UEditorAssetLibrary::DoesDirectoryExist(FolderPath) && !UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath) && !EmptyFoldersPathsArray.Contains(FolderPath))
			{
				EmptyFoldersPathsArray.Add(FolderPath);

				EmptyFoldersPathsNames.Append(TEXT("\n"));
				EmptyFoldersPathsNames.Append(FolderPath);
			}
		}
	}

	// Second check the selected folders
	for (const FString& FolderPathSelected : FoldersPathSelectedArray)
	{
		bool bAreAllSubfoldersEmpty = true;
		TArray<FString> SubfoldersPathArray = UEditorAssetLibrary::ListAssets(FolderPathSelected, false, true);
		for (const FString& FolderPath : SubfoldersPathArray)
		{
			if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath) || !EmptyFoldersPathsArray.Contains(FolderPath))
			{
				bAreAllSubfoldersEmpty = false;
				break;
			}
		}

		// Don't touch the root folder
		if (FolderPathSelected.Contains(TEXT("Developers")) || FolderPathSelected.Contains(TEXT("Collections"))
			|| FolderPathSelected.Contains(TEXT("__ExternalActors__")) || FolderPathSelected.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		if (UEditorAssetLibrary::DoesDirectoryExist(FolderPathSelected) && bAreAllSubfoldersEmpty && !EmptyFoldersPathsArray.Contains(FolderPathSelected))
		{
			EmptyFoldersPathsArray.Add(FolderPathSelected);

			EmptyFoldersPathsNames.Append(TEXT("\n"));
			EmptyFoldersPathsNames.Append(FolderPathSelected);
		}
	}

	if (EmptyFoldersPathsArray.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No empty folders found"));
		return;
	}

	EAppReturnType::Type ConfirmResult = DebugHeader::ShowMsgDialog(EAppMsgType::OkCancel, TEXT("Empty folders found ") + FString::FromInt(EmptyFoldersPathsArray.Num()) + TEXT(":") + EmptyFoldersPathsNames + TEXT("\n\nWould you like to delete all?"), false);
	if (ConfirmResult == EAppReturnType::Cancel)
	{
		return;
	}

	uint32 FoldersDeletedCounter = 0;
	for (const FString& EmptyFolderPath : EmptyFoldersPathsArray)
	{
		if (UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath))
		{
			++FoldersDeletedCounter;
		}
		else
		{
			DebugHeader::Print(TEXT("Failed to delete ") + EmptyFolderPath, FColor::Red);
		}
	}

	// Notify status
	FString ResultMessage = TEXT("Successfully deleted ") + FString::FromInt(FoldersDeletedCounter) + TEXT(" folders");
	if (FoldersDeletedCounter != EmptyFoldersPathsArray.Num())
	{
		ResultMessage.Append(TEXT("\nCouldn't delete ") + FString::FromInt(EmptyFoldersPathsArray.Num() - FoldersDeletedCounter) + TEXT(" folders"));
	}

	DebugHeader::ShowNotifyInfo(ResultMessage);
}

void FSuperManagerModule::OnAdvancedDeletionButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvancedDeletion"));
}

void FSuperManagerModule::FixUpRedirectors()
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

#pragma endregion

#pragma region

void FSuperManagerModule::RegisterAdvancedDeletionTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("AdvancedDeletion"), FOnSpawnTab::CreateRaw(this, &FSuperManagerModule::OnSpawnAdvancedDeletionTab)).SetDisplayName(FText::FromString(TEXT("Advanced Deletion")));
}

TSharedRef<SDockTab> FSuperManagerModule::OnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab).TabRole(ETabRole::NomadTab);
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)
