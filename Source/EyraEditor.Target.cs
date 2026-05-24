// Copyright Enhoney.

using UnrealBuildTool;
using System.Collections.Generic;

public class EyraEditorTarget : TargetRules
{
	public EyraEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange(new string[] {"EyraGame", "EyraEditor"});

		EyraGameTarget.ApplySharedEyraTargetSettings(this);

        // 构建适用于此目标类型的模块，用于CIS（配置信息标准）以及生成已安装的引擎版本
        if (!bBuildAllModules)
        {
            // 用于覆盖控制UCLASS和USTRUCT是否允许拥有原生指针成员的设置，如果不允许，那么就会出现UHT错误，并且应当使用TObjectPtr来代替
            NativePointerMemberBehaviorOverride = PointerMemberBehavior.Disallow;   // 不允许使用原生裸指针
        }

        // 此功能用于触屏设备开发，同时与“虚幻远程 2”应用程序配合使用
        EnablePlugins.Add("RemoteSession");
    }
}
