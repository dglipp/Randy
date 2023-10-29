#pragma once

#include <cstdint>
#include <string>

typedef enum memory_tag
{
    MEMORY_TAG_UNKNOWN,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_DARRAY,
    MEMORY_TAG_DICT,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BST,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    MEMORY_TAG_MAX_TAGS,
} memory_tag;

class MemoryInterface{

public:
    MemoryInterface();
    ~MemoryInterface();

    static uint64_t totalAllocated;
    static uint64_t taggedAllocations[MEMORY_TAG_MAX_TAGS];

    static void *allocate(size_t size, memory_tag tag);
    static void free(void *block, size_t size, memory_tag tag);
    static void *zeroMemory(void *block, size_t size, memory_tag tag);
    static void *copyMemory(void *dest, const void *source, size_t size);
    static void *setMemory(void *dest, int32_t value, size_t size);

    static std::string getMemoryUsageString();
};

