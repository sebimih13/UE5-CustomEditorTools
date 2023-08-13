// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/** Forward Declarations */
class FMenuBuilder;

class FSuperManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
#pragma region ContentBrowserMenuExtension

	void InitContentBrowserMenuExtension();

	TSharedRef<FExtender> CustomContentBrowserMenuExtender(const TArray<FString>& SelectedPaths);
	void AddContentBrowserMenuEntry(FMenuBuilder& MenuBuilder);
	
	void OnDeleteUnusedAssetsButtonClicked();
	void OnDeleteEmptyFoldersButtonClicked();
	void OnAdvancedDeletionButtonClicked();

	void FixUpRedirectors();

	TArray<FString> FoldersPathSelectedArray;

#pragma endregion

#pragma region

	void RegisterAdvancedDeletionTab();

	TSharedRef<SDockTab> OnSpawnAdvancedDeletionTab(const FSpawnTabArgs& SpawnTabArgs);

	TArray<TSharedPtr<FAssetData>> GetAllAssetsDataUnderSelectedFolder();

#pragma endregion

};
