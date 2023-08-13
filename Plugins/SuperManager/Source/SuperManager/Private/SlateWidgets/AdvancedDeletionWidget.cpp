// Fill out your copyright notice in the Description page of Project Settings.

#include "SlateWidgets/AdvancedDeletionWidget.h"
#include "Widgets/Layout/SScrollBox.h"

void SAdvancedDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	StoredAssetsDataArray = InArgs._AssetsDataToStoreArray;
	FSlateFontInfo TitleTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 30;

	ChildSlot
	[
		// Main vertical box
		SNew(SVerticalBox)

		// 1st vertical slot for title text
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
			.AutoHeight()
			[
				SNew(SScrollBox)

				+ SScrollBox::Slot()
			[
				SNew(SListView<TSharedPtr<FAssetData>>)
				.ItemHeight(24.0f)
				.ListItemsSource(&StoredAssetsDataArray)
				.OnGenerateRow(this, &SAdvancedDeletionTab::OnGenerateRowForList)
			]
		]

		// 4th Slot for 3 buttons
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]
	];
}

TSharedRef<ITableRow> SAdvancedDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
	SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
	[
		SNew(STextBlock)
		.Text(FText::FromString(AssetDataToDisplay->AssetName.ToString()))
	];

	return ListViewRowWidget;
}
