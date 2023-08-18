// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ISceneOutlinerColumn.h"

class FOutlinerSelectionLockColumn : public ISceneOutlinerColumn
{
public:
	/** Constructor */
	FOutlinerSelectionLockColumn(ISceneOutliner& SceneOutliner);

	virtual FName GetColumnID() override { return FName("SelectionLock"); }

	static FName GetID() { return FName("SelectionLock"); }

	virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override;

	virtual const TSharedRef< SWidget > ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem, const STableRow<FSceneOutlinerTreeItemPtr>& Row) override;

private:
	void OnRowWidgetCheckStateChanged(ECheckBoxState NewState, TWeakObjectPtr<AActor> CorrespondingActor);
};
