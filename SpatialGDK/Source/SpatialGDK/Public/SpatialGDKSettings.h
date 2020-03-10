// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "Utils/SpatialActorGroupManager.h"

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Misc/Paths.h"
#include "Utils/RPCContainer.h"

#include "SpatialGDKSettings.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpatialGDKSettings, Log, All);

class ASpatialDebugger;

/**
 * Enum that maps Unreal's log verbosity to allow use in settings.
**/
UENUM()
namespace ESettingsWorkerLogVerbosity
{
	enum Type
	{
		Fatal = 1,
		Error,
		Warning,
		Display,
		Log,
		Verbose,
		VeryVerbose,
	};
}

UENUM()
namespace EServicesRegion
{
	enum Type
	{
		Default,
		CN
	};
}

USTRUCT(BlueprintType)
struct FDistanceFrequencyPair
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpatialGDK")
	float DistanceRatio;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SpatialGDK")
	float Frequency;
};

UCLASS(config = SpatialGDKSettings, defaultconfig)
class SPATIALGDK_API USpatialGDKSettings : public UObject
{
	GENERATED_BODY()

public:
	USpatialGDKSettings(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	virtual void PostInitProperties() override;

	/** 
	 * The number of entity IDs to be reserved when the entity pool is first created. Ensure that the number of entity IDs
	 * reserved is greater than the number of Actors that you expect the server-worker instances to spawn at game deployment 
	*/
	UPROPERTY(EditAnywhere, config, Category = "Entity Pool", meta = (DisplayName = "Initial Entity ID Reservation Count"))
	uint32 EntityPoolInitialReservationCount;

	/** 
	 * Specifies when the SpatialOS Runtime should reserve a new batch of entity IDs: the value is the number of un-used entity 
	 * IDs left in the entity pool which triggers the SpatialOS Runtime to reserve new entity IDs
	*/
	UPROPERTY(EditAnywhere, config, Category = "Entity Pool", meta = (DisplayName = "Pool Refresh Threshold"))
	uint32 EntityPoolRefreshThreshold;

	/** 
	* Specifies the number of new entity IDs the SpatialOS Runtime reserves when `Pool refresh threshold` triggers a new batch.
	*/
	UPROPERTY(EditAnywhere, config, Category = "Entity Pool", meta = (DisplayName = "Refresh Count"))
	uint32 EntityPoolRefreshCount;

	/** Specifies the amount of time, in seconds, between heartbeat events sent from a game client to notify the server-worker instances that it's connected. */
	UPROPERTY(EditAnywhere, config, Category = "Heartbeat", meta = (DisplayName = "Heartbeat Interval (seconds)"))
	float HeartbeatIntervalSeconds;

	/** 
	* Specifies the maximum amount of time, in seconds, that the server-worker instances wait for a game client to send heartbeat events. 
	* (If the timeout expires, the game client has disconnected.) 
	*/
	UPROPERTY(EditAnywhere, config, Category = "Heartbeat", meta = (DisplayName = "Heartbeat Timeout (seconds)"))
	float HeartbeatTimeoutSeconds;

	/**
	* Same as HeartbeatTimeoutSeconds, but used if WITH_EDITOR is defined.
	*/
	UPROPERTY(EditAnywhere, config, Category = "Heartbeat", meta = (DisplayName = "Heartbeat Timeout With Editor (seconds)"))
	float HeartbeatTimeoutWithEditorSeconds;

	/**
	 * Specifies the maximum number of Actors replicated per tick.
	 * Default: `0` per tick  (no limit)
	 * (If you set the value to ` 0`, the SpatialOS Runtime replicates every Actor per tick; this forms a large SpatialOS  world, affecting the performance of both game clients and server-worker instances.)
	 * You can use the `stat Spatial` flag when you run project builds to find the number of calls to `ReplicateActor`, and then use this number for reference.
	 */
	UPROPERTY(EditAnywhere, config, Category = "Replication", meta = (DisplayName = "Maximum Actors replicated per tick"))
	uint32 ActorReplicationRateLimit;

	/** 
	* Specifies the maximum number of entities created by the SpatialOS Runtime per tick. 
	* (The SpatialOS Runtime handles entity creation separately from Actor replication to ensure it can handle entity creation requests under load.)
	* Note: if you set the value to 0, there is no limit to the number of entities created per tick. However, too many entities created at the same time might overload the SpatialOS Runtime, which can negatively affect your game.
	* Default: `0` per tick  (no limit)
	*/
	UPROPERTY(EditAnywhere, config, Category = "Replication", meta = (DisplayName = "Maximum entities created per tick"))
	uint32 EntityCreationRateLimit;

	/**
	 * When enabled, only entities which are in the net relevancy range of player controllers will be replicated to SpatialOS.
	 * This should only be used in single server configurations. The state of the world in the inspector will no longer be up to date.
	 */
	UPROPERTY(EditAnywhere, config, Category = "Replication", meta = (DisplayName = "Only Replicate Net Relevant Actors"))
	bool bUseIsActorRelevantForConnection;

	/**
	* Specifies the rate, in number of times per second, at which server-worker instance updates are sent to and received from the SpatialOS Runtime.
	* Default:1000/s
	*/
	UPROPERTY(EditAnywhere, config, Category = "Replication", meta = (DisplayName = "SpatialOS Network Update Rate"))
	float OpsUpdateRate;

	/** Replicate handover properties between servers, required for zoned worker deployments.*/
	UPROPERTY(EditAnywhere, config, Category = "Replication")
	bool bEnableHandover;

	/** Maximum NetCullDistanceSquared value used in Spatial networking. Set to 0.0 to disable. This is temporary and will be removed when the runtime issue is resolved.*/
	UPROPERTY(EditAnywhere, config, Category = "Replication")
	float MaxNetCullDistanceSquared;

	/** Seconds to wait before executing a received RPC substituting nullptr for unresolved UObjects*/
	UPROPERTY(EditAnywhere, config, Category = "Replication", meta = (DisplayName = "Wait Time Before Processing Received RPC With Unresolved Refs"))
	float QueuedIncomingRPCWaitTime;

	/** Seconds to wait before dropping an outgoing RPC.*/
	UPROPERTY(EditAnywhere, config, Category = "Replication", meta = (DisplayName = "Wait Time Before Dropping Outgoing RPC"))
	float QueuedOutgoingRPCWaitTime;

	/** Frequency for updating an Actor's SpatialOS Position. Updating position should have a low update rate since it is expensive.*/
	UPROPERTY(EditAnywhere, config, Category = "SpatialOS Position Updates")
	float PositionUpdateFrequency;

	/** Threshold an Actor needs to move, in centimeters, before its SpatialOS Position is updated.*/
	UPROPERTY(EditAnywhere, config, Category = "SpatialOS Position Updates")
	float PositionDistanceThreshold;

	/** Metrics about client and server performance can be reported to SpatialOS to monitor a deployments health.*/
	UPROPERTY(EditAnywhere, config, Category = "Metrics")
	bool bEnableMetrics;

	/** Display server metrics on clients.*/
	UPROPERTY(EditAnywhere, config, Category = "Metrics")
	bool bEnableMetricsDisplay;

	/** Frequency that metrics are reported to SpatialOS.*/
	UPROPERTY(EditAnywhere, config, Category = "Metrics", meta = (DisplayName = "Metrics Report Rate (seconds)"))
	float MetricsReportRate;

	/** 
	* By default the SpatialOS Runtime reports server-worker instance’s load in frames per second (FPS). 
	* Select this to switch so it reports as seconds per frame. 
	* This value is visible as 'Load' in the Inspector, next to each worker.
	*/
	UPROPERTY(EditAnywhere, config, Category = "Metrics")
	bool bUseFrameTimeAsLoad;

	/** Batch entity position updates to be processed on a single frame.*/
	UPROPERTY(config)
	bool bBatchSpatialPositionUpdates;

	/** Maximum number of ActorComponents/Subobjects of the same class that can be attached to an Actor.*/
	UPROPERTY(EditAnywhere, config, Category = "Schema Generation", meta = (DisplayName = "Maximum Dynamically Attached Subobjects Per Class"))
	uint32 MaxDynamicallyAttachedSubobjectsPerClass;

	/** EXPERIMENTAL - Adds granular result types for queries.
	Granular here means specifically the required Unreal components for spawning other actors and all data type components.
	Needs testing thoroughly before making default. May be replaced by component set result types instead. */
	UPROPERTY(config)
	bool bEnableResultTypes;

	/** Pack RPCs sent during the same frame into a single update. */
	UPROPERTY(config)
	bool bPackRPCs;

	/** The receptionist host to use if no 'receptionistHost' argument is passed to the command line. */
	UPROPERTY(EditAnywhere, config, Category = "Local Connection")
	FString DefaultReceptionistHost;

	UPROPERTY(EditAnywhere, Config, Category = "Region settings", meta = (ConfigRestartRequired = true, DisplayName = "Region where services are located"))
	TEnumAsByte<EServicesRegion::Type> ServicesRegion;

	/** Single server worker type to launch when offloading is disabled, fallback server worker type when offloading is enabled (owns all actor classes by default). */
	UPROPERTY(EditAnywhere, Config, Category = "Offloading")
	FWorkerType DefaultWorkerType;

	/** Enable running different server worker types to split the simulation by Actor Groups. Can be overridden with command line argument OverrideSpatialOffloading. */
	UPROPERTY(EditAnywhere, Config, Category = "Offloading")
	bool bEnableOffloading;

	/** Actor Group configuration. */
	UPROPERTY(EditAnywhere, Config, Category = "Offloading", meta = (EditCondition = "bEnableOffloading"))
	TMap<FName, FActorGroupInfo> ActorGroups;

	/** Available server worker types. */
	UPROPERTY(Config)
	TSet<FName> ServerWorkerTypes;

	/** Controls the verbosity of worker logs which are sent to SpatialOS. These logs will appear in the Spatial Output and launch.log */
	UPROPERTY(EditAnywhere, config, Category = "Logging", meta = (DisplayName = "Worker Log Level"))
	TEnumAsByte<ESettingsWorkerLogVerbosity::Type> WorkerLogLevel;

	UPROPERTY(EditAnywhere, config, Category = "Debug", meta = (MetaClass = "SpatialDebugger"))
	TSubclassOf<ASpatialDebugger> SpatialDebugger;

	/** EXPERIMENTAL: Disable runtime load balancing and use a worker to do it instead. */
	UPROPERTY(EditAnywhere, Config, Category = "Load Balancing")
	bool bEnableUnrealLoadBalancer;

	/** EXPERIMENTAL: Worker type to assign for load balancing. */
	UPROPERTY(EditAnywhere, Config, Category = "Load Balancing", meta = (EditCondition = "bEnableUnrealLoadBalancer"))
	FWorkerType LoadBalancingWorkerType;

	/** EXPERIMENTAL: Run SpatialWorkerConnection on Game Thread. */
	UPROPERTY(Config)
	bool bRunSpatialWorkerConnectionOnGameThread;

	/** RPC ring buffers is enabled when either the matching setting is set, or load balancing is enabled */
	bool UseRPCRingBuffer() const;

private:
#if WITH_EDITOR
	bool CanEditChange(const UProperty* InProperty) const override;
#endif

	UPROPERTY(EditAnywhere, Config, Category = "Replication", meta = (DisplayName = "Use RPC Ring Buffers"))
	bool bUseRPCRingBuffers;

	UPROPERTY(EditAnywhere, Config, Category = "Replication", meta = (DisplayName = "Default RPC Ring Buffer Size"))
	uint32 DefaultRPCRingBufferSize;

	/** Overrides default ring buffer size. */
	UPROPERTY(EditAnywhere, Config, Category = "Replication", meta = (DisplayName = "RPC Ring Buffer Size Map"))
	TMap<ERPCType, uint32> RPCRingBufferSizeMap;

public:
	uint32 GetRPCRingBufferSize(ERPCType RPCType) const;

	float GetSecondsBeforeWarning(const ERPCResult Result) const;

	/** The number of fields that the endpoint schema components are generated with. Changing this will require schema to be regenerated and break snapshot compatibility. */
	UPROPERTY(EditAnywhere, Config, Category = "Replication", meta = (DisplayName = "Max RPC Ring Buffer Size"))
	uint32 MaxRPCRingBufferSize;

	/** Only valid on Tcp connections - indicates if we should enable TCP_NODELAY - see c_worker.h */
	UPROPERTY(Config)
	bool bTcpNoDelay;

	/** Only valid on Udp connections - specifies server upstream flush interval - see c_worker.h */
	UPROPERTY(Config)
	uint32 UdpServerUpstreamUpdateIntervalMS;

	/** Only valid on Udp connections - specifies server downstream flush interval - see c_worker.h */
	UPROPERTY(Config)
	uint32 UdpServerDownstreamUpdateIntervalMS;

	/** Only valid on Udp connections - specifies client upstream flush interval - see c_worker.h */
	UPROPERTY(Config)
	uint32 UdpClientUpstreamUpdateIntervalMS;

	/** Only valid on Udp connections - specifies client downstream flush interval - see c_worker.h */
	UPROPERTY(Config)
	uint32 UdpClientDownstreamUpdateIntervalMS;

	/** Do async loading for new classes when checking out entities. */
	UPROPERTY(Config)
	bool bAsyncLoadNewClassesOnEntityCheckout;

	UPROPERTY(EditAnywhere, config, Category = "Queued RPC Warning Timeouts", AdvancedDisplay, meta = (DisplayName = "For a given RPC failure type, the time it will queue before reporting warnings to the logs."))
	TMap<ERPCResult, float> RPCQueueWarningTimeouts;

	UPROPERTY(EditAnywhere, config, Category = "Queued RPC Warning Timeouts", AdvancedDisplay, meta = (DisplayName = "Default time before a queued RPC will start reporting warnings to the logs."))
	float RPCQueueWarningDefaultTimeout;

	FORCEINLINE bool IsRunningInChina() const { return ServicesRegion == EServicesRegion::CN; }

	/** Enable to use the new net cull distance component tagging form of interest */
	UPROPERTY(EditAnywhere, Config, Category = "Interest")
	bool bEnableNetCullDistanceInterest;

	/** Enable to use interest frequency with bEnableNetCullDistanceInterest*/
	UPROPERTY(EditAnywhere, Config, Category = "Interest", meta = (EditCondition = "bEnableNetCullDistanceInterest"))
	bool bEnableNetCullDistanceFrequency;

	/** Full update frequency ratio of actor's net cull distance */
	UPROPERTY(EditAnywhere, Config, Category = "Interest", meta = (EditCondition = "bEnableNetCullDistanceFrequency"))
	float FullFrequencyNetCullDistanceRatio;

	/** QBI pairs for ratio of - net cull distance : update frequency */
	UPROPERTY(EditAnywhere, Config, Category = "Interest", meta = (EditCondition = "bEnableNetCullDistanceFrequency"))
	TArray<FDistanceFrequencyPair> InterestRangeFrequencyPairs;

	/** Use TLS encryption for UnrealClient workers connection. May impact performance. */
	UPROPERTY(EditAnywhere, Config, Category = "Connection")
	bool bUseSecureClientConnection;

	/** Use TLS encryption for UnrealWorker (server) workers connection. May impact performance. */
	UPROPERTY(EditAnywhere, Config, Category = "Connection")
	bool bUseSecureServerConnection;

	/**
	 * Enable to ensure server workers always express interest such that any server is interested in a super set of
	 * client interest. This will cause servers to make most of the same queries as their delegated client queries.
	 * Intended to be used in development before interest due to the LB strategy ensures correct functionality.
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Interest")
	bool bEnableClientQueriesOnServer;

	/** Experimental feature to use SpatialView layer when communicating with the Worker */
	UPROPERTY(Config)
	bool bUseSpatialView;

public:
	// UI Hidden settings passed through from SpatialGDKEditorSettings
	bool bUseDevelopmentAuthenticationFlow;
	FString DevelopmentAuthenticationToken;
	FString DevelopmentDeploymentToConnect;
};
