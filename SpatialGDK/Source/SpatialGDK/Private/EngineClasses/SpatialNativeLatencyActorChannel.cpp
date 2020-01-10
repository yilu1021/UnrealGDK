// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "EngineClasses/SpatialNativeLatencyActorChannel.h"

#include "Engine/DemoNetDriver.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/DataBunch.h"
#include "Net/NetworkProfiler.h"

#if WITH_EDITOR
#include "Settings/LevelEditorPlaySettings.h"
#endif

#include "EngineClasses/SpatialNetConnection.h"
#include "EngineClasses/SpatialNetDriver.h"
#include "EngineClasses/SpatialPackageMapClient.h"
#include "Interop/GlobalStateManager.h"
#include "Interop/SpatialReceiver.h"
#include "Interop/SpatialSender.h"
#include "LoadBalancing/AbstractLBStrategy.h"
#include "Schema/AlwaysRelevant.h"
#include "Schema/ClientRPCEndpointLegacy.h"
#include "Schema/ServerRPCEndpointLegacy.h"
#include "SpatialConstants.h"
#include "SpatialGDKSettings.h"
#include "Utils/RepLayoutUtils.h"
#include "Utils/SpatialActorUtils.h"

USpatialNativeLatencyActorChannel::USpatialNativeLatencyActorChannel(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
}
