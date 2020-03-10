// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "Schema/Component.h"
#include "Schema/PlayerSpawner.h"
#include "SpatialCommonTypes.h"
#include "SpatialConstants.h"
#include "Utils/SchemaUtils.h"

#include "Containers/UnrealString.h"

#include <WorkerSDK/improbable/c_schema.h>
#include <WorkerSDK/improbable/c_worker.h>

namespace SpatialGDK
{

// The ServerWorker component exists to hold the physical worker name corresponding to a
// server worker entity. This is so that the translator can make virtual workers to physical
// worker names using the server worker entities.
struct ServerWorker : Component
{
	static const Worker_ComponentId ComponentId = SpatialConstants::SERVER_WORKER_COMPONENT_ID;

	ServerWorker()
		: WorkerName(SpatialConstants::INVALID_WORKER_NAME)
		, bReadyToBeginPlay(false)
	{}

	ServerWorker(const PhysicalWorkerName& InWorkerName, const bool bInReadyToBeginPlay)
	{
		WorkerName = InWorkerName;
		bReadyToBeginPlay = bInReadyToBeginPlay;
	}

	ServerWorker(const Worker_ComponentData& Data)
	{
		Schema_Object* ComponentObject = Schema_GetComponentDataFields(Data.schema_type);

		WorkerName = GetStringFromSchema(ComponentObject, SpatialConstants::SERVER_WORKER_NAME_ID);
		bReadyToBeginPlay = GetBoolFromSchema(ComponentObject, SpatialConstants::SERVER_WORKER_READY_TO_BEGIN_PLAY_ID);
	}

	Worker_ComponentData CreateServerWorkerData()
	{
		Worker_ComponentData Data = {};
		Data.component_id = ComponentId;
		Data.schema_type = Schema_CreateComponentData();
		Schema_Object* ComponentObject = Schema_GetComponentDataFields(Data.schema_type);

		AddStringToSchema(ComponentObject, SpatialConstants::SERVER_WORKER_NAME_ID, WorkerName);
		Schema_AddBool(ComponentObject, SpatialConstants::SERVER_WORKER_READY_TO_BEGIN_PLAY_ID, bReadyToBeginPlay);

		return Data;
	}

	Worker_ComponentUpdate CreateServerWorkerUpdate()
	{
		Worker_ComponentUpdate Update = {};
		Update.component_id = ComponentId;
		Update.schema_type = Schema_CreateComponentUpdate();
		Schema_Object* ComponentObject = Schema_GetComponentUpdateFields(Update.schema_type);

		AddStringToSchema(ComponentObject, SpatialConstants::SERVER_WORKER_NAME_ID, WorkerName);
		Schema_AddBool(ComponentObject, SpatialConstants::SERVER_WORKER_READY_TO_BEGIN_PLAY_ID, bReadyToBeginPlay);

		return Update;
	}

	void ApplyComponentUpdate(const Worker_ComponentUpdate& Update)
	{
		Schema_Object* ComponentObject = Schema_GetComponentUpdateFields(Update.schema_type);

		WorkerName = GetStringFromSchema(ComponentObject, SpatialConstants::SERVER_WORKER_NAME_ID);
		bReadyToBeginPlay = GetBoolFromSchema(ComponentObject, SpatialConstants::SERVER_WORKER_READY_TO_BEGIN_PLAY_ID);
	}

	static Worker_CommandRequest CreateForwardPlayerSpawnRequest(Schema_CommandRequest* SchemaCommandRequest)
	{
		Worker_CommandRequest CommandRequest = {};
		CommandRequest.component_id = SpatialConstants::PLAYER_SPAWNER_COMPONENT_ID;
		CommandRequest.command_index = SpatialConstants::PLAYER_SPAWNER_SPAWN_PLAYER_COMMAND_ID;
		CommandRequest.schema_type = SchemaCommandRequest;
		return CommandRequest;
	}

	static Worker_CommandResponse CreateForwardPlayerSpawnReponse()
	{
		Worker_CommandResponse CommandResponse = {};
		CommandResponse.component_id = SpatialConstants::SERVER_WORKER_COMPONENT_ID;
		CommandResponse.command_index = SpatialConstants::SERVER_WORKER_FORWARD_SPAWN_REQUEST_COMMAND_ID;
		CommandResponse.schema_type = Schema_CreateCommandResponse();
		Schema_Object* ResponseObject = Schema_GetCommandResponseObject(CommandResponse.schema_type);
		return CommandResponse;
	}

	static void CreateForwardPlayerSpawnSchemaRequest(Schema_CommandRequest* Request, FUnrealObjectRef PlayerStartObjectRef, const Schema_Object* OriginalPlayerSpawnRequest)
	{
		Schema_Object* RequestFields = Schema_GetCommandRequestObject(Request);

		AddObjectRefToSchema(RequestFields, SpatialConstants::FORWARD_SPAWN_PLAYER_START_ACTOR_ID, PlayerStartObjectRef);

		Schema_Object* PlayerSpawnData = Schema_AddObject(RequestFields, SpatialConstants::FORWARD_SPAWN_PLAYER_DATA_ID);
		PlayerSpawner::CopySpawnDataBetweenObjects(OriginalPlayerSpawnRequest, PlayerSpawnData);
	}

	PhysicalWorkerName WorkerName;
	bool bReadyToBeginPlay;
};

} // namespace SpatialGDK

