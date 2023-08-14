// Fill out your copyright notice in the Description page of Project Settings.

#include "SlateWidgets/AdvancedDeletionWidget.h"
#include "Widgets/Layout/SScrollBox.h"
#include "SuperManagerModule.h"

// TODO : delete debug
#include "DebugHeader.h"

void SAdvancedDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	StoredAssetsDataArray = InArgs._AssetsDataToStoreArray;
	AssetsDataToDeleteArray.Empty();
	CheckBoxesArray.Empty();

	FSlateFontInfo TitleTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 30;

	ChildSlot
	[
		// Main vertical box
		SNew(SVerticalBox)

		// 1st Slot for title text
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Advanced Deletion")))
			.Font(TitleTextFont)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]

		// 2nd Slot for drop down to specify the listing condition
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]

		// 3rd Slot for the asset list
		+ SVerticalBox::Slot()
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			SNew(SScrollBox)

			+ SScrollBox::Slot()
			[
				ConstructAssetListView()
			]
		]

		// 4th Slot for 3 buttons
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			// Button 1: Delete All
			+SHorizontalBox::Slot()
			.FillWidth(10.0f)
			.Padding(5.0f)
			[
				ConstructDeleteAllButton()
			]

			// Button 2: Select All
			+SHorizontalBox::Slot()
			.FillWidth(10.0f)
			.Padding(5.0f)
			[
				ConstructSelectAllButton()
			]

			// Button 3: Deselect All
			+SHorizontalBox::Slot()
			.FillWidth(10.0f)
			.Padding(5.0f)
			[
				ConstructDeselectAllButton()
			]
		]
	];
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvancedDeletionTab::ConstructAssetListView()
{
	ConstructedAssetListView =
		SNew(SListView<TSharedPtr<FAssetData>>)
		.ItemHeight(24.0f)
		.ListItemsSource(&StoredAssetsDataArray)
		.OnGenerateRow(this, &SAdvancedDeletionTab::OnGenerateRowForList);

	return ConstructedAssetListView.ToSharedRef();
}

TSharedRef<ITableRow> SAdvancedDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	}

	FSlateFontInfo AssetClassNameTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	AssetClassNameTextFont.Size = 10;

	FSlateFontInfo AssetNameTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	AssetNameTextFont.Size = 15;

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
	SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
	.Padding(FMargin(5.0f))
	[
		SNew(SHorizontalBox)

		// 1st Slot for check box
		+SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Center)
		.FillWidth(0.05f)
		[
			ConstructCheckBox(AssetDataToDisplay)
		]

		// 2nd Slot for displaying asset class name
		+SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		.FillWidth(0.5f)
		[
			ConstructTextForRowWidget(AssetDataToDisplay->AssetClass.ToString(), AssetClassNameTextFont)
		]

		// 3rd Slot for displaying asset name
		+ SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Left)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			ConstructTextForRowWidget(AssetDataToDisplay->AssetName.ToString(), AssetNameTextFont)
		]

		// 4th Slot for a button
		+ SHorizontalBox::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			ConstructButtonForRowWidget(AssetDataToDisplay)
		]
	];

	return ListViewRowWidget;
}

void SAdvancedDeletionTab::RefreshAssetListView()
{
	AssetsDataToDeleteArray.Empty();
	CheckBoxesArray.Empty();

	if (ConstructedAssetListView.IsValid())
	{
		ConstructedAssetListView->RebuildList();
	}
}

TSharedRef<SCheckBox> SAdvancedDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData> AssetDataToDisplay)
{
	TSharedRef<SCheckBox> ConstructedCheckBox =
		SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this, &SAdvancedDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);

	CheckBoxesArray.Add(ConstructedCheckBox);
	return ConstructedCheckBox;
}

void SAdvancedDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		if (AssetsDataToDeleteArray.Contains(AssetData))
		{
			AssetsDataToDeleteArray.Remove(AssetData);
		}
		break;

	case ECheckBoxState::Checked:
		AssetsDataToDeleteArray.AddUnique(AssetData);
		break;

	case ECheckBoxState::Undetermined:
		break;

	default:
		break;
	}
}

TSharedRef<STextBlock> SAdvancedDeletionTab::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> ConstructedTextBlock =
		SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(FontToUse)
		.ColorAndOpacity(FColor::White);

	return ConstructedTextBlock;
}

TSharedRef<SButton> SAdvancedDeletionTab::ConstructButtonForRowWidget(TSharedPtr<FAssetData> AssetDataToDisplay)
{
	TSharedRef<SButton> ConstructedButton = 
		SNew(SButton)
		.Text(FText::FromString(TEXT("Delete")))
		.OnClicked(this, &SAdvancedDeletionTab::OnDeleteButtonClicked, AssetDataToDisplay);

	return ConstructedButton;
}

FReply SAdvancedDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
	bool bAssetDeleted = SuperManagerModule.DeleteSingleAssetForAssetList(*ClickedAssetData.Get());

	// Refresh the list
	if (bAssetDeleted)
	{
		// Update the list source items
		if (StoredAssetsDataArray.Contains(ClickedAssetData))
		{
			StoredAssetsDataArray.Remove(ClickedAssetData);
		}

		// Update the list
		RefreshAssetListView();
	}

	return FReply::Handled();
}

TSharedRef<SButton> SAdvancedDeletionTab::ConstructDeleteAllButton()
{
	TSharedRef<SButton> DeleteAllButton =
		SNew(SButton)
		.ContentPadding(FMargin(5.0f))
		.OnClicked(this, &SAdvancedDeletionTab::OnDeleteAllButtonClicked);

	DeleteAllButton->SetContent(ConstructTextBlockForTabButtons(TEXT("Delete All")));

	return DeleteAllButton;
}

FReply SAdvancedDeletionTab::OnDeleteAllButtonClicked()
{
	if (AssetsDataToDeleteArray.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No assets currently selected"));
		return FReply::Handled();
	}

	// Pass data to our module for deletion
	TArray<FAssetData> AssetDataToDelete;
	for (const TSharedPtr<FAssetData>& Data : AssetsDataToDeleteArray)
	{
		AssetDataToDelete.Add(*Data.Get());
	}

	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
	bool bAssetsDeleted = SuperManagerModule.DeleteMultipleAssetsForAssetList(AssetDataToDelete);

	if (bAssetsDeleted)
	{
		for (const TSharedPtr<FAssetData>& DeletedData : AssetsDataToDeleteArray)
		{
			if (StoredAssetsDataArray.Contains(DeletedData))
			{
				StoredAssetsDataArray.Remove(DeletedData);
			}
		}

		RefreshAssetListView();
	}

	return FReply::Handled();
}

TSharedRef<SButton> SAdvancedDeletionTab::ConstructSelectAllButton()
{
	TSharedRef<SButton> SelectAllButton =
		SNew(SButton)
		.ContentPadding(FMargin(5.0f))
		.OnClicked(this, &SAdvancedDeletionTab::OnSelectAllButtonClicked);

	SelectAllButton->SetContent(ConstructTextBlockForTabButtons(TEXT("Select All")));

	return SelectAllButton;
}

FReply SAdvancedDeletionTab::OnSelectAllButtonClicked()
{
	for (const TSharedRef<SCheckBox> CheckBox : CheckBoxesArray)
	{
		if (!CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}

	return FReply::Handled();
}

TSharedRef<SButton> SAdvancedDeletionTab::ConstructDeselectAllButton()
{
	TSharedRef<SButton> DeselectAllButton =
		SNew(SButton)
		.ContentPadding(FMargin(5.0f))
		.OnClicked(this, &SAdvancedDeletionTab::OnDeselectAllButtonClicked);

	DeselectAllButton->SetContent(ConstructTextBlockForTabButtons(TEXT("Deselect All")));

	return DeselectAllButton;
}

FReply SAdvancedDeletionTab::OnDeselectAllButtonClicked()
{
	for (const TSharedRef<SCheckBox> CheckBox : CheckBoxesArray)
	{
		if (CheckBox->IsChecked())
		{
			CheckBox->ToggleCheckedState();
		}
	}

	return FReply::Handled();
}

TSharedRef<STextBlock> SAdvancedDeletionTab::ConstructTextBlockForTabButtons(const FString& TextContent)
{
	FSlateFontInfo ButtonTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	ButtonTextFont.Size = 15;

	TSharedRef<STextBlock> ConstructedTextBlock =
		SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(ButtonTextFont)
		.Justification(ETextJustify::Center);

	return ConstructedTextBlock;
}
