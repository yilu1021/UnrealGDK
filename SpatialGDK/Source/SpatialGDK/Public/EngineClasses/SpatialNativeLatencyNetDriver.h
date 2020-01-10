// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "EngineClasses/SpatialLoadBalanceEnforcer.h"
#include "EngineClasses/SpatialVirtualWorkerTranslator.h"
#include "Interop/Connection/ConnectionConfig.h"
#include "Interop/SpatialDispatcher.h"
#include "Interop/SpatialOutputDevice.h"
#include "Interop/SpatialRPCService.h"
#include "Interop/SpatialSnapshotManager.h"
#include "Utils/SpatialActorGroupManager.h"

#include "LoadBalancing/AbstractLockingPolicy.h"
#include "SpatialConstants.h"
#include "SpatialGDKSettings.h"

#include "CoreMinimal.h"
#include "GameFramework/OnlineReplStructs.h"
#include "IpNetDriver.h"
#include "TimerManager.h"

#include "SpatialNativeLatencyNetDriver.generated.h"

UCLASS()
class SPATIALGDK_API USpatialNativeLatencyNetDriver : public UIpNetDriver
{
	GENERATED_BODY()

public:
	USpatialNativeLatencyNetDriver(const FObjectInitializer& ObjectInitializer);
	virtual bool InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error) override;
};
