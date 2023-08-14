// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"

class SAdvancedDeletionTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvancedDeletionTab) { }
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataToStoreArray)
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);
	void OnRowWidgetMouseButtonClicked(TSharedPtr<FAssetData> ClickedData);
	void RefreshAssetListView();

	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData> AssetDataToDisplay);
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

	TSharedRef<STextBlock> ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse);

	TSharedRef<SButton> ConstructButtonForRowWidget(TSharedPtr<FAssetData> AssetDataToDisplay);
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);

	TSharedRef<SButton> ConstructDeleteAllButton();
	FReply OnDeleteAllButtonClicked();

	TSharedRef<SButton> ConstructSelectAllButton();
	FReply OnSelectAllButtonClicked();

	TSharedRef<SButton> ConstructDeselectAllButton();
	FReply OnDeselectAllButtonClicked();

	TSharedRef<STextBlock> ConstructTextBlockForTabButtons(const FString& TextContent);
	
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();
	TSharedRef<SWidget> OnGenerateComboBoxContent(TSharedPtr<FString> SourceItem);
	void OnComboBoxSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo);

	/** Variables */
	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedAssetListView;
	TArray<TSharedPtr<FAssetData>> StoredAssetsDataArray;
	TArray<TSharedPtr<FAssetData>> DisplayedAssetsDataArray;

	TArray<TSharedPtr<FAssetData>> AssetsDataToDeleteArray;

	TArray<TSharedRef<SCheckBox>> CheckBoxesArray;

	TArray<TSharedPtr<FString>> ComboBoxSourceItems;
	TSharedPtr<STextBlock> ComboBoxDisplayTextBlock;
};
