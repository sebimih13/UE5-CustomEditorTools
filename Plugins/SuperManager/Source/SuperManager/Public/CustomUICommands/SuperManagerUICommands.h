// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Framework/Commands/Commands.h"

class FSuperManagerUICommands : public TCommands<FSuperManagerUICommands>
{
public:
	/** Constructor */
	FSuperManagerUICommands();

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> LockActorSelection;
	TSharedPtr<FUICommandInfo> UnlockActorSelection;
};
