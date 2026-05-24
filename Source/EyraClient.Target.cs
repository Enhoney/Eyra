// Copyright Enhoney. All Rights Reserved

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
