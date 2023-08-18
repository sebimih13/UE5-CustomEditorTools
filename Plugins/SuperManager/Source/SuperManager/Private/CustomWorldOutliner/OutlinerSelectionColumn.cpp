// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomWorldOutliner/OutlinerSelectionColumn.h"
#include "CustomStyle/SuperManagerStyle.h"
#include "ActorTreeItem.h"
#include "SuperManagerModule.h"

FOutlinerSelectionLockColumn::FOutlinerSelectionLockColumn(ISceneOutliner& SceneOutliner)
{

}

SHeaderRow::FColumn::FArguments FOutlinerSelectionLockColumn::ConstructHeaderRowColumn()
{
	SHeaderRow::FColumn::FArguments ConstructedHeaderRow = 
		SHeaderRow::Column(GetColumnID())
			.FixedWidth(24.0f)
			.HAlignHeader(EHorizontalAlignment::HAlign_Center)
			.VAlignHeader(EVerticalAlignment::VAlign_Center)
			.HAlignCell(EHorizontalAlignment::HAlign_Center)
			.VAlignCell(EVerticalAlignment::VAlign_Center)
			.DefaultTooltip(FText::FromString(TEXT("Actor selection lock - Press icon to lock actor selection")))
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
				.Image(FSuperManagerStyle::GetCreatedSlateStyleSet()->GetBrush(FName("LevelEditor.SelectionLock")))
			];

	return ConstructedHeaderRow;
}

const TSharedRef<SWidget> FOutlinerSelectionLockColumn::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row)
{
	FActorTreeItem* ActorTreeItem = TreeItem->CastTo<FActorTreeItem>();
	if (!ActorTreeItem || !ActorTreeItem->IsValid())
	{
		return SNullWidget::NullWidget;
	}

	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
	const bool bIsActorSelectionLocked = SuperManagerModule.CheckIsActorSelectionLocked(ActorTreeItem->Actor.Get());

	TSharedRef<SCheckBox> ConstructedRowWidgetCheckBox =
		SNew(SCheckBox)
		.Visibility(EVisibility::Visible)
		.Type(ESlateCheckBoxType::ToggleButton)
		.Style(&FSuperManagerStyle::GetCreatedSlateStyleSet()->GetWidgetStyle<FCheckBoxStyle>(FName("SceneOutliner.SelectionLock")))
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.IsChecked(bIsActorSelectionLocked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged(this, &FOutlinerSelectionLockColumn::OnRowWidgetCheckStateChanged, ActorTreeItem->Actor);

	return ConstructedRowWidgetCheckBox;
}

void FOutlinerSelectionLockColumn::OnRowWidgetCheckStateChanged(ECheckBoxState NewState, TWeakObjectPtr<AActor> CorrespondingActor)
{
	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		SuperManagerModule.ProcessLockingForOutliner(CorrespondingActor.Get(), false);
		break;

	case ECheckBoxState::Checked:
		SuperManagerModule.ProcessLockingForOutliner(CorrespondingActor.Get(), true);
		break;
	}
}
