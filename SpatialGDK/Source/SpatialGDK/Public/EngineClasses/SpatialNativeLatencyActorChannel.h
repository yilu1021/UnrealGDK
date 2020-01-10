// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "Engine/ActorChannel.h"

#include "EngineClasses/SpatialNetDriver.h"
#include "Interop/Connection/SpatialWorkerConnection.h"
#include "Interop/SpatialClassInfoManager.h"
#include "Interop/SpatialStaticComponentView.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Schema/StandardLibrary.h"
#include "SpatialCommonTypes.h"
#include "SpatialGDKSettings.h"
#include "Utils/RepDataUtils.h"

#include "SpatialNativeLatencyActorChannel.generated.h"

UCLASS(Transient)
class SPATIALGDK_API USpatialNativeLatencyActorChannel : public UActorChannel
{
	GENERATED_BODY()

public:
	USpatialNativeLatencyActorChannel(const FObjectInitializer & ObjectInitializer = FObjectInitializer::Get());
};
