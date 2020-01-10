// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "EngineClasses/SpatialNativeLatencyNetDriver.h"

#include "Engine/ActorChannel.h"
#include "Engine/ChildConnection.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/NetworkObjectList.h"
#include "EngineGlobals.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameNetworkManager.h"
#include "Net/DataReplication.h"
#include "Net/RepLayout.h"
#include "SocketSubsystem.h"
#include "UObject/UObjectIterator.h"

#include "EngineClasses/SpatialNativelatencyActorChannel.h"
#include "EngineClasses/SpatialGameInstance.h"
#include "EngineClasses/SpatialNetConnection.h"
#include "EngineClasses/SpatialPackageMapClient.h"
#include "EngineClasses/SpatialPendingNetGame.h"
#include "Interop/Connection/SpatialWorkerConnection.h"
#include "Interop/GlobalStateManager.h"
#include "Interop/SpatialClassInfoManager.h"
#include "Interop/SpatialPlayerSpawner.h"
#include "Interop/SpatialReceiver.h"
#include "Interop/SpatialSender.h"
#include "Interop/SpatialWorkerFlags.h"
#include "LoadBalancing/AbstractLBStrategy.h"
#include "LoadBalancing/GridBasedLBStrategy.h"
#include "LoadBalancing/ReferenceCountedLockingPolicy.h"
#include "Schema/AlwaysRelevant.h"
#include "SpatialConstants.h"
#include "SpatialGDKSettings.h"
#include "Utils/EntityPool.h"
#include "Utils/ErrorCodeRemapping.h"
#include "Utils/InterestFactory.h"
#include "Utils/OpUtils.h"
#include "Utils/SpatialDebugger.h"
#include "Utils/SpatialMetrics.h"
#include "Utils/SpatialMetricsDisplay.h"
#include "Utils/SpatialStatics.h"

#if WITH_EDITOR
#include "Settings/LevelEditorPlaySettings.h"
#include "SpatialGDKServicesModule.h"
#endif

USpatialNativeLatencyNetDriver::USpatialNativeLatencyNetDriver(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool USpatialNativeLatencyNetDriver::InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error)
{
	if (!Super::InitBase(bInitAsClient, InNotify, URL, bReuseAddressAndPort, Error))
	{
		return false;
	}

	
	// Make absolutely sure that the actor channel that we are using is our Spatial actor channel
	// Copied from what the Engine does with UActorChannel
	FChannelDefinition SpatialChannelDefinition{};
	SpatialChannelDefinition.ChannelName = NAME_Actor;
	SpatialChannelDefinition.ClassName = FName(*USpatialNativeLatencyActorChannel::StaticClass()->GetPathName());
	SpatialChannelDefinition.ChannelClass = USpatialNativeLatencyActorChannel::StaticClass();
	SpatialChannelDefinition.bServerOpen = true;

	ChannelDefinitions[CHTYPE_Actor] = SpatialChannelDefinition;
	ChannelDefinitionMap[NAME_Actor] = SpatialChannelDefinition;

	return true;
}
