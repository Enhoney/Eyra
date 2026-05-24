// Copyright Enhoney. All Rights Reserved

using UnrealBuildTool;

public class EyraEditor : ModuleRules
{
	public EyraEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"EyraEditor",
			}
		);
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);
	
		PublicDependencyModuleNames.AddRange(
			new string[] {
				// UE 基础类型（字符串、容器等）
				"Core",

				// UObject 系统（反射、GC 等）
				"CoreUObject",

				// 引擎核心功能（渲染、物理、输入等）
				"Engine",

				// 编辑器扩展框架（自定义资产编辑器等）
				"EditorFramework",

				// 虚幻编辑器核心功能（关卡编辑、细节面板等）
				"UnrealEd",

				// 物理系统基础支持
				"PhysicsCore",

				// GameplayTag的编辑器支持（如标签管理页面）
				"GameplayTagsEditor",

				// 游戏任务系统的编辑器工具
				"GameplayTasksEditor",

				// GAS的运行时逻辑
				"GameplayAbilities",

				// GA系统的编辑器工具（如技能蓝图编辑）
				"GameplayAbilitiesEditor",

				// 开发工作室的遥测数据收集（用于分析开发行为）
				"StudioTelemetry",

				// Eyra游戏本身的运行时模块（依赖游戏逻辑）
				"EyraGame"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				// 输入设备（键盘、鼠标）支持
				"InputCore",

				// UE的UI框架（编辑器界面元素）
				"Slate",
				"SlateCore",

				// 编辑器工具栏和菜单扩展
				"ToolMenus",

				// 编辑器UI样式（图标、字体等）
				"EditorStyle",

				// 数据校验工具（防止无效资产）
				"DataValidation",

				// 编辑器日志系统
				"MessageLog",

				// 项目管理（插件、游戏路径等）
				"Projects",

				// 开发者工具配置
				"DeveloperToolSettings",

				// 资产集合管理（分类、标签等）
				"CollectionManager",

				// 版本控制集成（如Git、Perforce等）
				"SourceControl",

				// Chaos物理引擎支持
				"Chaos"
            }
        );

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				// 可能需要动态加载的模块（如特定编辑器插件）
			}
		);

        // 外部RPC框架的基本设置
        // 在正式发布系统中，框架内的功能将被删除，以消除潜在的安全漏洞
        PrivateDependencyModuleNames.Add("ExternalRpcRegistry");
        if (Target.Configuration == UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.Add("WITH_RPC_REGISTRY=0");
            PublicDefinitions.Add("WITH_HTTPSERVER_LISTENERS=0");
        }
		else
		{
            // HTTP服务器功能（仅开发/测试版启用）
            PrivateDependencyModuleNames.Add("HTTPServer");
            PublicDefinitions.Add("WITH_RPC_REGISTRY=1");
            PublicDefinitions.Add("WITH_HTTPSERVER_LISTENERS=1");
        }

        // 若在测试版本或正式版本中使用DrawDebug函数，则会生成编译错误
        PublicDefinitions.Add("SHIPPING_DRAW_DEBUG_ERROR=1");
    }
}
