// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/** Forward Declarations */
class FMenuBuilder;
class UEditorActorSubsystem;
class FUICommandList;

class FSuperManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Process Data For Advanced Deletion Tab */
	bool DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete);
	bool DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsDataToDeleteArray);
	void ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter, TArray<TSharedPtr<FAssetData>>& OutUnusedAssetsData);
	void ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter, TArray<TSharedPtr<FAssetData>>& OutSameNameAssetsData);
	void SyncContentBrowserToClickedAssetForAssetList(const FString& AssetPathToSync);

private:
	/** ContentBrowserMenuExtension */
	void InitContentBrowserMenuExtension();
	TSharedRef<FExtender> CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths);
	void AddContentBrowserMenuEntry(FMenuBuilder& MenuBuilder);
	
	void OnDeleteUnusedAssetsButtonClicked();
	void OnDeleteEmptyFoldersButtonClicked();
	void OnAdvancedDeletionButtonClicked();

	void FixUpRedirectors();

	TArray<FString> FoldersPathSelectedArray;

	/** Custom Editor Tab */
	void RegisterAdvancedDeletionTab();
	void UnregisterAdvancedDeletionTab();
	TSharedRef<SDockTab> OnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
	TArray<TSharedPtr<FAssetData>> GetAllAssetsDataUnderSelectedFolder();

	/** Level Editor Menu Extension */
	void InitLevelEditorMenuExtension();
	TSharedRef<FExtender> CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList, const TArray<AActor*> SelectedActorsArray);
	void AddLevelEditorMenuEntry(FMenuBuilder& MenuBuilder);

	void OnLockActorSelectionButtonClicked();
	void OnUnlockActorSelectionButtonClicked();

	/** Selection Lock */
	void InitCustomSelectionEvent();
	void OnActorSelected(UObject* SelectedObject);

	void LockActorSelection(AActor* ActorToProcess);
	void UnlockActorSelection(AActor* ActorToProcess);
	bool CheckIsActorSelectionLocked(AActor* ActorToProcess);

	TWeakObjectPtr<UEditorActorSubsystem> WeakEditorActorSubsystem;
	
	bool GetEditorActorSubsystem();

	/** Custom Editor UI Commands */
	void InitCustomUICommands();
	void OnLockActorSelectionHotKeyPressed();
	void OnUnlockActorSelectionHotKeyPressed();

	TSharedPtr<FUICommandList> CustomUICommands;
};
