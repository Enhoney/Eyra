// Copyright Enhoney. All Rights Reserved

#pragma once

#include "CommonGameInstance.h"

#include "EyraGameInstance.generated.h"


#define UE_API EYRAGAME_API

// TODO：需要替换成我们自定义的PC类
// class AEyraPlayerController;
class APlayerController;
class UObject;


UCLASS(MinimalAPI, Config = Game)
class UEyraGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

public:

	UE_API UEyraGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// TODO：需要替换成我们自定义的PC类
	UE_API APlayerController* GetPrimaryPlayerController() const;

	/** Checks if the requested session can be joined. Can be overridden per game. */
	// 检查所请求的会话是否可以加入。可根据每场比赛进行覆盖设置
	UE_API virtual bool CanJoinRequestedSession() const override;

	// 处理用户初始化
	UE_API virtual void HandlerUserInitialized(const UCommonUserInfo* UserInfo, bool bSuccess, 
		FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext) override;

	// 处理加密密钥的设置事宜，若游戏需要自行实现此功能，则必须在自身（可能为异步）处理完成时调用此委托函数
	UE_API virtual void ReceivedNetworkEncryptionToken(const FString& EncryptionToken, const FOnEncryptionKeyResponse& Delegate) override;

	// 当客户端从服务器接收到“加密确认”控制消息时会调用此函数，通常会启动加密功能
	UE_API virtual void ReceivedNetworkEncryptionAck(const FOnEncryptionKeyResponse& Delegate) override;

protected:
	// 初始化
	UE_API virtual void Init() override;
	// 注销
	UE_API virtual void Shutdown() override;

	// 当客户端跳转到服务器会话关卡时，我们可以在这里重写URL，携带令牌参数
	UE_API void OnPreClientTravelToSession(FString& URL);

	/** A hard-coded encryption key used to try out the encryption code. This is NOT SECURE, do not use this technique in production! */
	// 用于测试加密代码的硬编码加密密钥。此方法不安全，请勿在生产环境中使用。
	TArray<uint8> DebugTestEncryptionKey;
};

#undef UE_API