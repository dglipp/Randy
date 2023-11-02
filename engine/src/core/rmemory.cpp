#include "core/rmemory.h"

#include "core/logger.h"
#include "platform/platform.h"

#include <format>

const char * memory_tag_strings[MEMORY_TAG_MAX_TAGS]
{
    "UNKNOWN     ",
    "ARRAY       ",
    "DARRAY      ",
    "DICT        ",
    "RING_QUEUE  ",
    "BST         ",
    "STRING      ",
    "APPLICATION ",
    "JOB         ",
    "TEXTURE     ",
    "MAT_INSTANCE",
    "RENDERER    ",
    "GAME        ",
    "TRANSFORM   ",
    "ENTITY      ",
    "ENTITY_NODE ",
    "SCENE       ",
};

uint64_t MemoryInterface::totalAllocated = 0;
uint64_t MemoryInterface::taggedAllocations[] = {0};

MemoryInterface::MemoryInterface()
{
}

MemoryInterface::~MemoryInterface(){};

std::string MemoryInterface::getMemoryUsageString()
{
    const float_t gib = 1024 * 1024 * 1024;
    const float_t mib = 1024 * 1024;
    const float_t kib = 1024;

    std::string output = "System memory use (tagged):\n";
    std::string unit;
    float_t amount;

    for (int i = 0; i < MEMORY_TAG_MAX_TAGS; ++i)
    {
        if (MemoryInterface::taggedAllocations[i] >= gib)
        {
            unit = "GiB";
            amount = MemoryInterface::taggedAllocations[i] / gib;
        }
        else if (MemoryInterface::taggedAllocations[i] >= mib)
        {
            unit = "MiB";
            amount = MemoryInterface::taggedAllocations[i] / mib;
        }
        else if (MemoryInterface::taggedAllocations[i] >= kib)
        {
            unit = "KiB";
            amount = MemoryInterface::taggedAllocations[i] / kib;
        }
        else
        {
            unit = "B";
            amount = (float_t) MemoryInterface::taggedAllocations[i];
        }

        output += std::format("{}: {:.2f}{}\n", memory_tag_strings[i], amount, unit);
    }

    return output;
}

void *MemoryInterface::allocate(size_t size, memory_tag tag)
{
    if (tag == MEMORY_TAG_UNKNOWN)
    {
        R_WARN("Allocate called using MEMMORY_TAG_UNKNOWN!");
    }

    MemoryInterface::totalAllocated += size;
    MemoryInterface::taggedAllocations[tag] += size;

    // TODO:: memory alignment
    void *block = Platform::allocate(size, false);
    Platform::zeroMemory(block, size);

    return block;
}

void MemoryInterface::free(void *block, size_t size, memory_tag tag)
{
    if (tag == MEMORY_TAG_UNKNOWN)
    {
        R_WARN("Allocate called using MEMMORY_TAG_UNKNOWN!");
    }

    MemoryInterface::totalAllocated -= size;
    MemoryInterface::taggedAllocations[tag] -= size;

    // TODO:: memory alignment
    Platform::freeMemory(block, false);
}

void *MemoryInterface::zeroMemory(void *block, size_t size)
{
    return Platform::zeroMemory(block, size);
}

void *MemoryInterface::copyMemory(void *dest, const void *source, size_t size)
{
    return Platform::copyMemory(dest, source, size);
}

void *MemoryInterface::setMemory(void *dest, int32_t value, size_t size)
{
    return Platform::setMemory(dest, value, size);
}