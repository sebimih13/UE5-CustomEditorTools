// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/SlateStyle.h"

class FSuperManagerStyle
{
public:
	static void InitializeIcons();
	static void Shutdown();

	FORCEINLINE static FName GetStyleSetName() { return StyleSetName; }

private:
	static TSharedRef<FSlateStyleSet> CreateSlateStyleSet();

	static const FName StyleSetName;
	static TSharedPtr<FSlateStyleSet> CreatedSlateStyleSet;
};
