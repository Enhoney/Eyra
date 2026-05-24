// Copyright Enhoney.
using UnrealBuildTool;
using System.Collections.Generic;

public class EyraServerTarget : TargetRules
{
	public EyraServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		ExtraModuleNames.Add("EyraGame");

        EyraGameTarget.ApplySharedEyraTargetSettings(this);

        // 是否在测试版本/发布版本开启检查（断言）功能
        bUseChecksInShipping = true; // 发布版本开启断言功能
    }
}
