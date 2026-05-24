// Copyright Enhoney.

using UnrealBuildTool;
using System.Collections.Generic;

public class EyraClientTarget : TargetRules
{
	public EyraClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		ExtraModuleNames.Add("EyraGame");

		EyraGameTarget.ApplySharedEyraTargetSettings(this);
	}
}
