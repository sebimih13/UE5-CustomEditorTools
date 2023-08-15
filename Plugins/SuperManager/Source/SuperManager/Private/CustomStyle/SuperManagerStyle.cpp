// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomStyle/SuperManagerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

// Initialize static variables
const FName FSuperManagerStyle::StyleSetName = FName("SuperManagerStyle");
TSharedPtr<FSlateStyleSet> FSuperManagerStyle::CreatedSlateStyleSet = nullptr;

void FSuperManagerStyle::InitializeIcons()
{
	if (!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
	}
}

void FSuperManagerStyle::Shutdown()
{
	if (CreatedSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
		CreatedSlateStyleSet.Reset();
	}
}

TSharedRef<FSlateStyleSet> FSuperManagerStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));
	const FString ResourcesDirectory = IPluginManager::Get().FindPlugin(TEXT("SuperManager"))->GetBaseDir() /"Resources";

	const FVector2D Icon16x16(16.0f, 16.0f);

	CustomStyleSet->SetContentRoot(ResourcesDirectory);
	CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets", new FSlateImageBrush(ResourcesDirectory/"DeleteUnusedAsset.png", Icon16x16));
	CustomStyleSet->Set("ContentBrowser.DeleteEmptyFolders", new FSlateImageBrush(ResourcesDirectory/"DeleteEmptyFolders.png", Icon16x16));
	CustomStyleSet->Set("ContentBrowser.AdvancedDeletion", new FSlateImageBrush(ResourcesDirectory/"AdvancedDeletion.png", Icon16x16));

	return CustomStyleSet;
}
