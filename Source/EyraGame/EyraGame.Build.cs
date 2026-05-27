// Copyright Enhoney. All Rights Reserved

using UnrealBuildTool;

public class EyraGame : ModuleRules
{
	public EyraGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"EyraGame"
			}
		
		);

		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);

        PublicDependencyModuleNames.AddRange(
			new string[] {
				// UE基础类型和反射系统
				"Core", 
				"CoreUObject",

				// 在线功能基础支持（如登录、会话管理）
				"CoreOnline",

				// 应用程序级功能（如窗口管理、输入事件等）
				"ApplicationCore",

				// 引擎核心（Actor、关卡、渲染等）
				"Engine",

				// 物理系统基础支持
				"PhysicsCore",

				// GameplayTag系统（用于技能、状态分类）
				"GameplayTags",

				// 异步任务系统（如技能流程、AI行为等）
				"GameplayTasks",

				// GAS的核心逻辑
				"GameplayAbilities",

				// AI行为树、导航系统
				"AIModule",

				// 模块化游戏设计（动态组合游戏功能）
				"ModularGameplay",

				// 支持模块化设计的Actor类
				"ModularGameplayActors",

				// 数据驱动注册表（动态加载配置数据）
				"DataRegistry",

				// 优化的网络复制系统（大型地图同步）
				"ReplicationGraph",

				// 游戏功能插件系统（动态加载DLC/模块）
				"GameFeatures",

				// 重要度管理系统（优化性能，优先处理重要事件）
				"SignificanceManager",

				// 热修复
				"Hotfix",

				// 通用加载屏幕管理
				"CommonLoadingScreen",

				// 粒子特效系统
				"Niagara",

				// 异步操作支持（混合到其他类中）
				"AsyncMixin",

				// 复杂逻辑流程控制（如技能链，剧情分支）
				"ControlFlows",

				// 提供动态访问和修改对象属性的功能
				"PropertyPath"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				// 输入设备支持
                "InputCore",

				// UI基础框架（非游戏内UI，如编辑器工具）
                "Slate",
                "SlateCore",

				// 渲染核心
                "RenderCore",

				// 开发者设置
                "DeveloperSettings",

				// 增强输入系统（更灵活的输入映射和处理）
                "EnhancedInput",

				// 网络核心
                "NetCore",

				// RHI模块 底层图形API抽象层，封装了DirectX、Vulkan等图形API的接口，提供统一的渲染接口。
				// 管理纹理、缓冲区、渲染目标等资源的创建和使用，处理渲染命令的提交和执行，优化渲染性能。
                "RHI",

				// 项目管理（插件、游戏路径等）
                "Projects",

				// 自动化测试框架
                "Gauntlet",

				// 游戏内UI
                "UMG",

				// 通用UI组件（如菜单、HUD）
                "CommonUI",

				// 跨平台输入统一管理
                "CommonInput",

				// 游戏设置
                "GameSettings",

				// 通用游戏逻辑（如玩家管理、游戏模式）
                "CommonGame",

				// 用户账户与权限管理
                "CommonUser",

				// 字幕
                "GameSubtitles",

				// 游戏内消息总线（事件广播）
                "GameplayMessageRuntime",

				// 高级音频混合与控制
                "AudioMixer",

				// 网络回放功能（回放录制与回放播放）
                "NetworkReplayStreaming",

				// UI接入点
                "UIExtension",

				// 客户端自动化行为控制
                "ClientPilot",

				// 音频调制系统（动态调整音频参数）
                "AudioModulation",

				// 引擎设置（项目配置和运行时设置）
                "EngineSettings",

				// 加密网络通信支持（安全传输层）
                "DTLSHandlerComponent",

				// JSON序列化与反序列化支持（配置文件、网络数据等）
                "Json",
            }
		);

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
            }
        );

        // 若在测试版本或正式版本中使用DrawDebug函数，则会生成编译错误
        PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");

        // 外部RPC框架的基本设置
        // 在正式发布系统中，框架内的功能将被删除，以消除潜在的安全漏洞
        PrivateDependencyModuleNames.Add("ExternalRpcRegistry");
        PrivateDependencyModuleNames.Add("HTTPServer"); // Dependency for ExternalRpcRegistry
        if (Target.Configuration == UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("WITH_RPC_REGISTRY=0");
            PublicDefinitions.Add("WITH_HTTPSERVER_LISTENERS=0");
        }
        else
        {
            PublicDefinitions.Add("WITH_RPC_REGISTRY=1");
            PublicDefinitions.Add("WITH_HTTPSERVER_LISTENERS=1");
        }

		// 为游戏调试器支持此模块
        SetupGameplayDebuggerSupport(Target);
        // 根据UEBuildConfiguration设置，为Iris网络支持此模块
        SetupIrisSupport(Target);
    }
}
