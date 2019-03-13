

bool CreateAllocator(const AllocatorCreateInfo *pCreateInfo, ResourceAllocator **pAllocator) {
  ASSERT(pCreateInfo && pAllocator);
  RESOURCE_DEBUG_LOG("resourceAllocCreateAllocator");
  *pAllocator = conf_placement_new<ResourceAllocator>(AllocatorAllocate<ResourceAllocator>(), pCreateInfo);
  return true;
}

void DestroyAllocator(ResourceAllocator *allocator) {
  if (allocator != RESOURCE_NULL) {
    RESOURCE_DEBUG_LOG("resourceAllocDestroyAllocator");
    resourceAlloc_delete(allocator);
  }
}

void ResourceAllocCalculateStats(ResourceAllocator *allocator, AllocatorStats *pStats) {
  ASSERT(allocator && pStats);
  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK
  allocator->CalculateStats(pStats);
}

#if RESOURCE_STATS_STRING_ENABLED

void ResourceAllocBuildStatsString(ResourceAllocator* allocator, char** ppStatsString, uint32_t detailedMap)
{
  ASSERT(allocator && ppStatsString);
  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  AllocatorStringBuilder sb(allocator);
  {
    AllocatorStats stats;
    allocator->CalculateStats(&stats);

    sb.Add("{\n\"Total\": ");
    AllocatorPrintStatInfo(sb, stats.total);

    for (uint32_t heapIndex = 0; heapIndex < allocator->GetMemoryHeapCount(); ++heapIndex)
    {
      sb.Add(",\n\"Heap ");
      sb.AddNumber(heapIndex);
      sb.Add("\": {\n\t\"Size\": ");
      //sb.AddNumber(allocator->m_MemProps.memoryHeaps[heapIndex].size);
      sb.Add(",\n\t\"Flags\": ");
      //if (gHeapProperties[heapIndex].mProps.Type == D3D12_HEAP_TYPE_DEFAULT)
      //{
      //  sb.AddString("DEVICE_LOCAL");
      //}
      //else
      //{
      //  sb.AddString("");
      //}
      if (stats.memoryHeap[heapIndex].AllocationCount > 0)
      {
        sb.Add(",\n\t\"Stats:\": ");
        AllocatorPrintStatInfo(sb, stats.memoryHeap[heapIndex]);
      }

      for (uint32_t typeIndex = 0; typeIndex < allocator->GetMemoryTypeCount(); ++typeIndex)
      {
        //if (allocator->m_MemProps.memoryTypes[typeIndex].heapIndex == heapIndex)
        {
          sb.Add(",\n\t\"Type ");
          sb.AddNumber(typeIndex);
          sb.Add("\": {\n\t\t\"Flags\": \"");
          /*if (gHeapProperties[typeIndex].mProps.Type == D3D12_HEAP_TYPE_DEFAULT)
          {
              sb.Add(" DEVICE_LOCAL");
          }
          if (gHeapProperties[typeIndex].mProps.Type == D3D12_HEAP_TYPE_UPLOAD)
          {
              sb.Add(" HOST_VISIBLE");
          }
          if (gHeapProperties[typeIndex].mProps.Type == D3D12_HEAP_TYPE_READBACK)
          {
              sb.Add(" HOST_COHERENT");
          }*/
          sb.Add("\"");
          if (stats.memoryType[typeIndex].AllocationCount > 0)
          {
            sb.Add(",\n\t\t\"Stats\": ");
            AllocatorPrintStatInfo(sb, stats.memoryType[typeIndex]);
          }
          sb.Add("\n\t}");
        }
      }
      sb.Add("\n}");
    }
    if (detailedMap == 1)
    {
      allocator->PrintDetailedMap(sb);
    }
    sb.Add("\n}\n");
  }

  const size_t len = sb.GetLength();
  char* const  pChars = conf_placement_new<char>(AllocatorAllocateArray<char>((len + 1)));
  if (len > 0)
  {
    memcpy(pChars, sb.GetData(), len);
  }
  pChars[len] = '\0';
  *ppStatsString = pChars;
}

void ResourceAllocFreeStatsString(ResourceAllocator* allocator, char* pStatsString)
{
  if (pStatsString != RESOURCE_NULL)
  {
    ASSERT(allocator);
    size_t len = strlen(pStatsString);
    resourceAlloc_delete_array(pStatsString, len + 1);
  }
}

#endif    // #if RESOURCE_STATS_STRING_ENABLED

bool ResourceAllocMapMemory(ResourceAllocator *allocator, ResourceAllocation *allocation, void **ppData) {
  ASSERT(allocator && allocation && ppData);

  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  if (allocation->GetResource()) {
    *ppData = allocation->GetResource().contents;
  }

  return false;
}

void ResourceAllocUnmapMemory(ResourceAllocator *allocator, ResourceAllocation *allocation) {
  ASSERT(allocator && allocation);

  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  //if (allocation->GetResource()) allocation->GetResource()->Unmap(0, NULL);
}

void ResourceAllocUnmapPersistentlyMappedMemory(ResourceAllocator *allocator) {
  ASSERT(allocator);

  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  allocator->UnmapPersistentlyMappedMemory();
}

bool ResourceAllocMapPersistentlyMappedMemory(ResourceAllocator *allocator) {
  ASSERT(allocator);

  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  return allocator->MapPersistentlyMappedMemory();
}

bool ResourceAllocFindSuballocType(MTLTextureDescriptor *desc, AllocatorSuballocationType *suballocType) {
  *suballocType = RESOURCE_SUBALLOCATION_TYPE_UNKNOWN;
  if (desc.usage & MTLTextureUsageRenderTarget) {
    *suballocType = RESOURCE_SUBALLOCATION_TYPE_IMAGE_RTV_DSV;
  } else {
    *suballocType = RESOURCE_SUBALLOCATION_TYPE_IMAGE_OPTIMAL;
  }

  return (*suballocType != RESOURCE_SUBALLOCATION_TYPE_UNKNOWN) ? true : false;
}

void ResourceAllocFreeMemory(ResourceAllocator *allocator, ResourceAllocation *allocation) {
  ASSERT(allocator && allocation);

  RESOURCE_DEBUG_LOG("resourceAllocFreeMemory");

  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  allocator->FreeMemory(allocation);
}

void ResourceAllocGetAllocationInfo(ResourceAllocator *allocator,
                                    ResourceAllocation *allocation,
                                    ResourceAllocationInfo *pAllocationInfo) {
  ASSERT(allocator && allocation && pAllocationInfo);

  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  allocator->GetAllocationInfo(allocation, pAllocationInfo);
}

void ResourceAllocSetAllocationUserData(ResourceAllocator *allocator, ResourceAllocation *allocation, void *pUserData) {
  ASSERT(allocator && allocation);

  RESOURCE_DEBUG_GLOBAL_MUTEX_LOCK

  allocation->SetUserData(pUserData);
}
