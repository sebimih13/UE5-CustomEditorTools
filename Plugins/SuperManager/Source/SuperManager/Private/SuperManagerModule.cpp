// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManagerModule.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "SlateWidgets/AdvancedDeletionWidget.h"
#include "CustomStyle/SuperManagerStyle.h"
#include "LevelEditor.h"
#include "Engine/Selection.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "CustomUICommands/SuperManagerUICommands.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
	FSuperManagerStyle::InitializeIcons();

	InitContentBrowserMenuExtension();
	RegisterAdvancedDeletionTab();

	FSuperManagerUICommands::Register();
	InitCustomUICommands();

	InitLevelEditorMenuExtension();
	InitCustomSelectionEvent();
}

void FSuperManagerModule::ShutdownModule()
{
	FSuperManagerUICommands::Unregister();
	UnregisterAdvancedDeletionTab();
	FSuperManagerStyle::Shutdown();
}

bool FSuperManagerModule::DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete)
{
	TArray<FAssetData> AssetDataForDeletionArray;
	AssetDataForDeletionArray.Add(AssetDataToDelete);

	if (ObjectTools::DeleteAssets(AssetDataForDeletionArray) > 0)
	{
		return true;
	}
	return false;
}

bool FSuperManagerModule::DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsDataToDeleteArray)
{
	if (ObjectTools::DeleteAssets(AssetsDataToDeleteArray) > 0)
	{
		return true;
	}
	return false;
}

void FSuperManagerModule::ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter, TArray<TSharedPtr<FAssetData>>& OutUnusedAssetsData)
{
	OutUnusedAssetsData.Empty();

	for (const TSharedPtr<FAssetData>& AssetData : AssetsDataToFilter)
	{
		TArray<FString> AssetReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetData->ObjectPath.ToString());
		if (AssetReferences.Num() == 0)
		{
			OutUnusedAssetsData.Add(AssetData);
		}
	}
}

void FSuperManagerModule::ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter, TArray<TSharedPtr<FAssetData>>& OutSameNameAssetsData)
{
	OutSameNameAssetsData.Empty();

	TMultiMap<FString, TSharedPtr<FAssetData>> AssetsMap;
	for (const TSharedPtr<FAssetData>& AssetData : AssetsDataToFilter)
	{
		AssetsMap.Emplace(AssetData->AssetName.ToString(), AssetData);
	}

	TArray<FString> MapKeys;
	AssetsMap.GetKeys(MapKeys);

	for (const FString& Key : MapKeys)
	{
		TArray<TSharedPtr<FAssetData>> AssetsDataWithSameNameArray;
		AssetsMap.MultiFind(Key, AssetsDataWithSameNameArray);

		if (AssetsDataWithSameNameArray.Num() > 1)
		{
			for (const TSharedPtr<FAssetData>& AssetData : AssetsDataWithSameNameArray)
			{
				if (AssetData.IsValid())
				{
					OutSameNameAssetsData.AddUnique(AssetData);
				}
			}
		}
	}
}

void FSuperManagerModule::SyncContentBrowserToClickedAssetForAssetList(const FString& AssetPathToSync)
{
	TArray<FString> AssetsPathToSync;
	AssetsPathToSync.Add(AssetPathToSync);
	UEditorAssetLibrary::SyncBrowserToObjects(AssetsPathToSync);
}

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
	TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());

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
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.DeleteUnusedAssets"),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetsButtonClicked)
	);

	// Delete empty folders
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete empty folders")),
		FText::FromString(TEXT("Safely delete all empty folders and subfolders")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.DeleteEmptyFolders"),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked)
	);

	// Advanced Deletion
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Advanced Deletion")),
		FText::FromString(TEXT("List assets by specific conditions in a tab for deleting")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.AdvancedDeletion"),
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
	FixUpRedirectors();
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

void FSuperManagerModule::RegisterAdvancedDeletionTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		FName("AdvancedDeletion"), 
		FOnSpawnTab::CreateRaw(this, &FSuperManagerModule::OnSpawnAdvancedDeletionTab))
			.SetDisplayName(FText::FromString(TEXT("Advanced Deletion")))
			.SetIcon(FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.AdvancedDeletion")
	);
}

void FSuperManagerModule::UnregisterAdvancedDeletionTab()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("AdvancedDeletion"));
}

TSharedRef<SDockTab> FSuperManagerModule::OnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	if (FoldersPathSelectedArray.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("You must right click on a folder from Content Browser"));
		return
			SNew(SDockTab)
			.TabRole(ETabRole::NomadTab);
	}

	TSharedRef<SDockTab> CreatedAdvancedDeletionTab = 
		SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SAdvancedDeletionTab)
			.AssetsDataToStoreArray(GetAllAssetsDataUnderSelectedFolder())
			.CurrentSelectedFolder(FoldersPathSelectedArray[0])
		];

	return CreatedAdvancedDeletionTab;
}

TArray<TSharedPtr<FAssetData>> FSuperManagerModule::GetAllAssetsDataUnderSelectedFolder()
{
	TArray<TSharedPtr<FAssetData>> AvailableAssetsDataArray;

	TArray<FString> AssetsPathNameArray = UEditorAssetLibrary::ListAssets(FoldersPathSelectedArray[0]);
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

		const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPathName);
		AvailableAssetsDataArray.Add(MakeShared<FAssetData>(Data));
	}

	return AvailableAssetsDataArray;
}

void FSuperManagerModule::InitLevelEditorMenuExtension()
{
	// Get all menu extenders
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	TArray<FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors>& LevelEditorMenuExtenders = LevelEditorModule.GetAllLevelViewportContextMenuExtenders();

	// Add the new custom commands
	TSharedRef<FUICommandList> ExistingLevelCommands = LevelEditorModule.GetGlobalLevelEditorActions();
	ExistingLevelCommands->Append(CustomUICommands.ToSharedRef());

	// Add a new custom delegate
	LevelEditorMenuExtenders.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateRaw(this, &FSuperManagerModule::CustomLevelEditorMenuExtender));
}

TSharedRef<FExtender> FSuperManagerModule::CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList, const TArray<AActor*> SelectedActorsArray)
{
	TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());

	if (SelectedActorsArray.Num() > 0)
	{
		MenuExtender->AddMenuExtension(TEXT("ActorOptions"), EExtensionHook::Before, UICommandList, FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddLevelEditorMenuEntry));
	}

	return MenuExtender;
}

void FSuperManagerModule::AddLevelEditorMenuEntry(FMenuBuilder& MenuBuilder)
{
	// Lock Actor Selection
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(TEXT("Lock actor selection")),
		FText::FromString(TEXT("Prevent actor from being selected")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.SelectionLock"),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnLockActorSelectionButtonClicked)
	);

	// Unlock Actor Selection
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Unlock actor selection")),
		FText::FromString(TEXT("Remove the selection constraint on this actor")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.SelectionUnlock"),
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnUnlockActorSelectionButtonClicked)
	);
}

void FSuperManagerModule::OnLockActorSelectionButtonClicked()
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	TArray<AActor*> SelectedActorsArray = WeakEditorActorSubsystem->GetSelectedLevelActors();
	if (SelectedActorsArray.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	FString CurrentLockedActorsNames = TEXT("Locked selection for:");
	for (AActor* SelectedActor : SelectedActorsArray)
	{
		if (!SelectedActor)
		{
			return;
		}

		LockActorSelection(SelectedActor);
		WeakEditorActorSubsystem->SetActorSelectionState(SelectedActor, false);

		CurrentLockedActorsNames.Append(TEXT("\n"));
		CurrentLockedActorsNames.Append(SelectedActor->GetActorLabel());
	}

	DebugHeader::ShowNotifyInfo(CurrentLockedActorsNames);
}

void FSuperManagerModule::OnUnlockActorSelectionButtonClicked()
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	TArray<AActor*> AllLevelActorsArray = WeakEditorActorSubsystem->GetAllLevelActors();
	TArray<AActor*> AllLockedActorsArray;

	for (AActor* LevelActor : AllLevelActorsArray)
	{
		if (!LevelActor)
		{
			return;
		}

		if (CheckIsActorSelectionLocked(LevelActor))
		{
			AllLockedActorsArray.Add(LevelActor);
		}
	}

	if (AllLockedActorsArray.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No selection locked actor"));
	}

	FString UnlockedActorsNames = TEXT("Lifted selection constraint for:");
	for (AActor* LockedActor : AllLockedActorsArray)
	{
		UnlockActorSelection(LockedActor);

		UnlockedActorsNames.Append(TEXT("\n"));
		UnlockedActorsNames.Append(LockedActor->GetActorLabel());
	}

	DebugHeader::ShowNotifyInfo(UnlockedActorsNames);
}

void FSuperManagerModule::InitCustomSelectionEvent()
{
	USelection* UserSelection = GEditor->GetSelectedActors();
	UserSelection->SelectObjectEvent.AddRaw(this, &FSuperManagerModule::OnActorSelected);
}

void FSuperManagerModule::OnActorSelected(UObject* SelectedObject)
{
	if (!GetEditorActorSubsystem())
	{
		return;
	}

	if (AActor* SelectedActor = Cast<AActor>(SelectedObject))
	{
		if (CheckIsActorSelectionLocked(SelectedActor))
		{
			// Deselect the actor
			WeakEditorActorSubsystem->SetActorSelectionState(SelectedActor, false);
		}
	}
}

void FSuperManagerModule::LockActorSelection(AActor* ActorToProcess)
{
	if (!ActorToProcess)
	{
		return;
	}

	if (!ActorToProcess->ActorHasTag(FName("Locked")))
	{
		ActorToProcess->Tags.Add(FName("Locked"));
	}
}

void FSuperManagerModule::UnlockActorSelection(AActor* ActorToProcess)
{
	if (!ActorToProcess)
	{
		return;
	}

	if (ActorToProcess->ActorHasTag(FName("Locked")))
	{
		ActorToProcess->Tags.Remove(FName("Locked"));
	}
}

bool FSuperManagerModule::CheckIsActorSelectionLocked(AActor* ActorToProcess)
{
	if (!ActorToProcess)
	{
		return false;
	}

	return ActorToProcess->ActorHasTag(FName("Locked"));
}

bool FSuperManagerModule::GetEditorActorSubsystem()
{
	if (!WeakEditorActorSubsystem.IsValid())
	{
		WeakEditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
	}

	return WeakEditorActorSubsystem.IsValid();
}

void FSuperManagerModule::InitCustomUICommands()
{
	CustomUICommands = MakeShareable(new FUICommandList());
	CustomUICommands->MapAction(FSuperManagerUICommands::Get().LockActorSelection, FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnLockActorSelectionHotKeyPressed));
	CustomUICommands->MapAction(FSuperManagerUICommands::Get().UnlockActorSelection, FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnUnlockActorSelectionHotKeyPressed));
}

void FSuperManagerModule::OnLockActorSelectionHotKeyPressed()
{
	OnLockActorSelectionButtonClicked();
}

void FSuperManagerModule::OnUnlockActorSelectionHotKeyPressed()
{
	OnUnlockActorSelectionButtonClicked();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)
