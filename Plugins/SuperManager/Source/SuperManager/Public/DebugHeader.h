#pragma once

#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

void Print(const FString& Message, const FColor& Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
	}
}

void PrintLog(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgAsWarning = true)
{
	if (bShowMsgAsWarning)
	{
		FText MessageTitle = FText::FromString(TEXT("Warning"));
		return FMessageDialog::Open(MsgType, FText::FromString(Message), &MessageTitle);
	}
	else
	{
		return FMessageDialog::Open(MsgType, FText::FromString(Message));
	}
}

void ShowNotifyInfo(const FString& Message)
{
	FNotificationInfo NotifyInfo(FText::FromString(Message));
	NotifyInfo.bUseLargeFont = true;
	NotifyInfo.FadeOutDuration = 7.0f;

	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}
