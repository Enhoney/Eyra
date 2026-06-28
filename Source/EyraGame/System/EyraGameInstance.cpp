// Copyright Enhoney. All Rights Reserved

#include "EyraGameInstance.h"

#include "CommonSessionSubsystem.h"
#include "CommonUserSubsystem.h"
#include "Components/GameFrameworkComponentManager.h"
#include "HAL/IConsoleManager.h"
#include "EyraGameplayTags.h"
#include "Misc/Paths.h"
// #include "Player/EyraPlayerController.h"
// #include "Player/EyraLocalPlayer.h"
#include "GameFramework/PlayerState.h"

#if UE_WITH_DTLS
#include "DTLSCertStore.h"
#include "DTLSHandlerComponent.h"
#include "Misc/FileHelper.h"
#endif // UE_WITH_DTLS

#include UE_INLINE_GENERATED_CPP_BY_NAME(EyraGameInstance)

UEyraGameInstance::UEyraGameInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

APlayerController* UEyraGameInstance::GetPrimaryPlayerController() const
{
    return Super::GetPrimaryPlayerController(false);
    // TODO：需要替换成我们自定义的PC类
    // return Cast<ARyraPlayerController>(Super::GetPrimaryPlayerController(false))
}

bool UEyraGameInstance::CanJoinRequestedSession() const
{
    return Super::CanJoinRequestedSession();
}

void UEyraGameInstance::HandlerUserInitialized(const UCommonUserInfo* UserInfo, 
    bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext)
{
    Super::HandlerUserInitialized(UserInfo, bSuccess, Error, RequestedPrivilege, OnlineContext);

    // TODO：等我们自定义的LocalPlayer添加好释放注释
    // If login succeeded, tell the local player to load their settings
    // 如果登录成功，请告诉本地玩家加载他们的设置
    //if (bSuccess && ensure(UserInfo))
    //{
    //    ULyraLocalPlayer* LocalPlayer = Cast<ULyraLocalPlayer>(GetLocalPlayerByIndex(UserInfo->LocalPlayerIndex));

    //    // There will not be a local player attached to the dedicated server user
    //    // 专用服务器用户不会关联本地玩家
    //    if (LocalPlayer)
    //    {
    //        LocalPlayer->LoadSharedSettingsFromDisk();
    //    }
    //}
}

void UEyraGameInstance::ReceivedNetworkEncryptionToken(const FString& EncryptionToken, const FOnEncryptionKeyResponse& Delegate)
{
    Super::ReceivedNetworkEncryptionToken(EncryptionToken, Delegate);
}

void UEyraGameInstance::ReceivedNetworkEncryptionAck(const FOnEncryptionKeyResponse& Delegate)
{
    Super::ReceivedNetworkEncryptionAck(Delegate);
}

void UEyraGameInstance::Init()
{
    Super::Init();


    // Register our custom init states
    // 注册自定义初始化状态
    UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

    // 注册一个全局的状态
    // 它形成了这样的一条状态链
    //(null)→ Spawned → DataAvailable → DataInitialized → GameplayReady
    //    ↑                                                     ↑
    //    起点                                                 终点：可以开始玩游戏
    // ComponentManager 会强制校验：要进入某个状态，必须先满足它的前置状态。比如你想进入 DataInitialized，系统会检查你是否已处于 DataAvailable。
    // 这是 Lyra 实现"复杂 Actor 多组件协调初始化"的核心机制。
    /**
    * 实际工作流程（以 Lyra 为例）
    * 每个继承了 IGameFrameworkInitStateInterface 的 Component 会覆写 CanChangeInitState() 来定义业务条件：
    *
    * PawnExtensionComponent 的条件：
    *
    * → Spawned：必须挂在一个有效的 Pawn 上
    * → DataAvailable：PawnData 已设置 + 被 Controller 拥有
    * → DataInitialized：Pawn 上所有其他 feature 都达到 DataAvailable
    * → GameplayReady：无条件放行
    * HeroComponent 的条件：
    *
    * → DataAvailable：PlayerState 存在 + Controller 与 PlayerState 配对 + InputComponent 就绪
    *
    * → DataInitialized：PawnExtensionComponent 已达到 DataInitialized
    * → GameplayReady：AbilitySystemComponent 已初始化
    */
    if (ensure(ComponentManager))
    {
        ComponentManager->RegisterInitState(EyraGameplayTags::InitState_Spawned, false, FGameplayTag());
        ComponentManager->RegisterInitState(EyraGameplayTags::InitState_DataAvailable, false, EyraGameplayTags::InitState_Spawned);
        ComponentManager->RegisterInitState(EyraGameplayTags::InitState_DataInitialized, false, EyraGameplayTags::InitState_DataAvailable);
        ComponentManager->RegisterInitState(EyraGameplayTags::InitState_GameplayReady, false, EyraGameplayTags::InitState_DataInitialized);
    }

    // Initialize the debug key with a set value for AES256. This is not secure and for example purposes only.
    // 使用预设值初始化调试密钥，用于AES256加密算法。此操作不安全，仅作示例用途。
    DebugTestEncryptionKey.SetNum(32);

    for (int32 i = 0; i < DebugTestEncryptionKey.Num(); ++i)
    {
        DebugTestEncryptionKey[i] = uint8(i);
    }

    // 在客户端访问服务器URL前重写URL 添加令牌校验参数
    if (UCommonSessionSubsystem* SessionSubsystem = GetSubsystem<UCommonSessionSubsystem>())
    {
        SessionSubsystem->OnPreClientTravelEvent.AddUObject(this, &UEyraGameInstance::OnPreClientTravelToSession);
    }
}

void UEyraGameInstance::Shutdown()
{
    if (UCommonSessionSubsystem* SessionSubsystem = GetSubsystem<UCommonSessionSubsystem>())
    {
        SessionSubsystem->OnPreClientTravelEvent.RemoveAll(this);
    }

    Super::Shutdown();
}

void UEyraGameInstance::OnPreClientTravelToSession(FString& URL)
{
    
}
