// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomUICommands/SuperManagerUICommands.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

FSuperManagerUICommands::FSuperManagerUICommands()
	: TCommands<FSuperManagerUICommands>(FName("SuperManager"), FText::FromString(TEXT("Super Manger UI Commands")), NAME_None, FName("SuperManager"))
{

}

void FSuperManagerUICommands::RegisterCommands()
{
	UI_COMMAND(
		LockActorSelection, 
		"Lock Actor Selection", 
		"Lock Actor Selection in level, once triggered, actor can no longer be selected", 
		EUserInterfaceActionType::Button, 
		FInputChord(EKeys::W, EModifierKey::Alt),	
	);

	UI_COMMAND(
		UnlockActorSelection, 
		"Unlock Actor Selection", 
		"Remove selection lock on all actors", 
		EUserInterfaceActionType::Button, 
		FInputChord(EKeys::W, EModifierKey::Alt | EModifierKey::Shift),	
	);
}

#undef LOCTEXT_NAMESPACE
