// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetActions/QuickMaterialCreationWidget.h"
#include "DebugHeader.h"

UQuickMaterialCreationWidget::UQuickMaterialCreationWidget()
	: MaterialName(TEXT("M_"))
{

}

void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	DebugHeader::Print(TEXT("Working"), FColor::Cyan);
}
