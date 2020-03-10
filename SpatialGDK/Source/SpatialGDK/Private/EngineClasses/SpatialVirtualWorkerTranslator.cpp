// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#include "EngineClasses/SpatialVirtualWorkerTranslator.h"
#include "LoadBalancing/AbstractLBStrategy.h"
#include "SpatialConstants.h"
#include "Utils/SchemaUtils.h"

DEFINE_LOG_CATEGORY(LogSpatialVirtualWorkerTranslator);

SpatialVirtualWorkerTranslator::SpatialVirtualWorkerTranslator(UAbstractLBStrategy* InLoadBalanceStrategy,
	PhysicalWorkerName InPhysicalWorkerName)
	: LoadBalanceStrategy(InLoadBalanceStrategy)
	, bIsReady(false)
	, LocalPhysicalWorkerName(InPhysicalWorkerName)
	, LocalVirtualWorkerId(SpatialConstants::INVALID_VIRTUAL_WORKER_ID)
{}

const PhysicalWorkerName* SpatialVirtualWorkerTranslator::GetPhysicalWorkerForVirtualWorker(VirtualWorkerId Id) const
{
	if (const TPair<PhysicalWorkerName, Worker_EntityId>* PhysicalWorkerInfo = VirtualToPhysicalWorkerMapping.Find(Id))
	{
		return &PhysicalWorkerInfo->Get<0>();
	}

	return nullptr;
}

Worker_EntityId SpatialVirtualWorkerTranslator::GetServerWorkerEntityForVirtualWorker(VirtualWorkerId Id) const
{
	if (const TPair<PhysicalWorkerName, Worker_EntityId>* PhysicalWorkerInfo = VirtualToPhysicalWorkerMapping.Find(Id))
	{
		return PhysicalWorkerInfo->Get<1>();
	}

	return SpatialConstants::INVALID_ENTITY_ID;
}

void SpatialVirtualWorkerTranslator::ApplyVirtualWorkerManagerData(Schema_Object* ComponentObject)
{
    UE_LOG(LogSpatialVirtualWorkerTranslator, Log, TEXT("(%d) ApplyVirtualWorkerManagerData"), LocalVirtualWorkerId);

	// The translation schema is a list of Mappings, where each entry has a virtual and physical worker ID. 
	ApplyMappingFromSchema(ComponentObject);

	for (const auto& Entry : VirtualToPhysicalWorkerMapping)
	{
		UE_LOG(LogSpatialVirtualWorkerTranslator, Log, TEXT("Translator assignment: Virtual Worker %d to %s with server worker entity: %lld"), Entry.Key, *(Entry.Value.Get<0>()), Entry.Value.Get<1>());
	}
}

// Check to see if this worker's physical worker name is in the mapping. If it isn't, it's possibly an old mapping.
// This is needed to give good behaviour across restarts. It's not very efficient, but it should happen only a few times
// after a PiE restart.
bool SpatialVirtualWorkerTranslator::IsValidMapping(Schema_Object* Object) const
{
	int32 TranslationCount = (int32)Schema_GetObjectCount(Object, SpatialConstants::VIRTUAL_WORKER_TRANSLATION_MAPPING_ID);

	for (int32 i = 0; i < TranslationCount; i++)
	{
		// Get each entry of the list and then unpack the virtual and physical IDs from the entry.
		Schema_Object* MappingObject = Schema_IndexObject(Object, SpatialConstants::VIRTUAL_WORKER_TRANSLATION_MAPPING_ID, i);
		if (SpatialGDK::GetStringFromSchema(MappingObject, SpatialConstants::MAPPING_PHYSICAL_WORKER_NAME) == LocalPhysicalWorkerName)
		{
			VirtualWorkerId ReceivedVirtualWorkerId = Schema_GetUint32(MappingObject, SpatialConstants::MAPPING_VIRTUAL_WORKER_ID);
			if (LocalVirtualWorkerId != SpatialConstants::INVALID_VIRTUAL_WORKER_ID && LocalVirtualWorkerId != ReceivedVirtualWorkerId)
			{
				UE_LOG(LogSpatialVirtualWorkerTranslator, Error, TEXT("Received mapping containing a new and updated virtual worker ID, this shouldn't happen."));
				return false;
			}
			return true;
		}
	}

	return false;
}

// The translation schema is a list of Mappings, where each entry has a virtual and physical worker ID.
// This method should only be called on workers who are not authoritative over the mapping and also when
// a worker first becomes authoritative for the mapping.
void SpatialVirtualWorkerTranslator::ApplyMappingFromSchema(Schema_Object* Object)
{
	if (!IsValidMapping(Object))
	{
		UE_LOG(LogSpatialVirtualWorkerTranslator, Log, TEXT("Received invalid mapping, likely due to PiE restart, will wait for a valid version."));
		return;
	}

	// Resize the map to accept the new data.
	VirtualToPhysicalWorkerMapping.Empty();
	int32 TranslationCount = (int32)Schema_GetObjectCount(Object, SpatialConstants::VIRTUAL_WORKER_TRANSLATION_MAPPING_ID);
	VirtualToPhysicalWorkerMapping.Reserve(TranslationCount);

	for (int32 i = 0; i < TranslationCount; i++)
	{
		// Get each entry of the list and then unpack the virtual and physical IDs from the entry.
		Schema_Object* MappingObject = Schema_IndexObject(Object, SpatialConstants::VIRTUAL_WORKER_TRANSLATION_MAPPING_ID, i);
		VirtualWorkerId VirtualWorkerId = Schema_GetUint32(MappingObject, SpatialConstants::MAPPING_VIRTUAL_WORKER_ID);
		PhysicalWorkerName PhysicalWorkerName = SpatialGDK::GetStringFromSchema(MappingObject, SpatialConstants::MAPPING_PHYSICAL_WORKER_NAME);
		Worker_EntityId ServerWorkerEntityId = Schema_GetEntityId(MappingObject, SpatialConstants::MAPPING_SERVER_WORKER_ENTITY_ID);

		// Insert each into the provided map.
		UpdateMapping(VirtualWorkerId, PhysicalWorkerName, ServerWorkerEntityId);
	}
}

void SpatialVirtualWorkerTranslator::UpdateMapping(VirtualWorkerId Id, PhysicalWorkerName Name, Worker_EntityId ServerWorkerEntityId)
{
	VirtualToPhysicalWorkerMapping.Add(Id, MakeTuple(Name, ServerWorkerEntityId));

	if (LocalVirtualWorkerId == SpatialConstants::INVALID_VIRTUAL_WORKER_ID && Name == LocalPhysicalWorkerName)
	{
		LocalVirtualWorkerId = Id;
		bIsReady = true;

		// Tell the strategy about the local virtual worker id. This is an "if" and not a "check" to allow unit tests which don't
		// provide a strategy.
		if (LoadBalanceStrategy.IsValid())
		{
			LoadBalanceStrategy->SetLocalVirtualWorkerId(LocalVirtualWorkerId);
		}

		UE_LOG(LogSpatialVirtualWorkerTranslator, Log, TEXT("VirtualWorkerTranslator is now ready for loadbalancing."));
	}
}
