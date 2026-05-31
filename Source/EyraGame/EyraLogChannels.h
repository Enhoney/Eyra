// Copyright Enhoney. All Rights Reserved

#pragma once

#include "Logging/LogMacros.h"

class UObject;

// 游戏模块通用日志
DECLARE_LOG_CATEGORY_EXTERN(LogEyraGame, Log, All);

// Experience系统日志
DECLARE_LOG_CATEGORY_EXTERN(LogEyraExperience, Log, All);

// AbilitySystem系统日志
DECLARE_LOG_CATEGORY_EXTERN(LogEyraAbilitySystem, Log, All);

// 组队系统日志
DECLARE_LOG_CATEGORY_EXTERN(LogEyraTeam, Log, All);

// 游戏阶段日志
DECLARE_LOG_CATEGORY_EXTERN(LogEyraGamePhase, Log, All);

// GM日志
DECLARE_LOG_CATEGORY_EXTERN(LogEyraCheat, Log, All);

// 游戏配置注册相关日志
DECLARE_LOG_CATEGORY_EXTERN(LogEyraGameSettingRegistry, Log, Log);

// ReplicationGraph日志
DECLARE_LOG_CATEGORY_EXTERN(LogEyraRepGraph, Display, All);


// 传递一个上下文对象，判断当前世界是否是服务器，客户端，或者单机，并返回一个字符串描述
// 该函数在EyraExperienceManagerComponent中使用，用来打印Experience的加载日志，快递的对象是GameState，该对象具有网络同步的属性
FString GetClientServerContextString(UObject* ContextObject = nullptr);
