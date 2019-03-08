
#include "core/core.h"
#include "core/logger.h"
#include "dynamic_allocator.hpp"
#include "stats.hpp"

// -------------------------------------------------------------------------------------------------
// AllocatorStringBuilder class.
// -------------------------------------------------------------------------------------------------

#if RESOURCE_STATS_STRING_ENABLED
void AllocatorUint32ToStr(char* outStr, size_t strLen, uint32_t num)
{
  char*    p;         /* pointer to traverse string */
  char*    firstdig;  /* pointer to first digit */
  char     temp;      /* temp char */
  uint32_t val = num; /* full value */
  uint32_t digval;    /* value of digit */

  p = outStr;
  firstdig = p; /* save pointer to first digit */
  do
  {
    digval = (uint32_t)(num % 10UL);
    num /= 10UL; /* get next digit */

    /* convert to ascii and store */
    if (digval > 9)
      *p++ = (char)(digval - 10 + 'a'); /* a letter */
    else
      *p++ = (char)(digval + '0'); /* a digit */
  } while (val > 0);

  /* We now have the digit of the number in the buffer, but in reverse
   order.  Thus we reverse them now. */
  *p-- = '\0'; /* terminate string; p points to last digit */
  do
  {
    temp = *p;
    *p = *firstdig;
    *firstdig = temp; /* swap *p and *firstdig */
    --p;
    ++firstdig;         /* advance to next two digits */
  } while (firstdig < p); /* repeat until halfway */
}

void AllocatorUint64ToStr(char* outStr, size_t strLen, uint64_t num)
{
  char*              p;         /* pointer to traverse string */
  char*              firstdig;  /* pointer to first digit */
  char               temp;      /* temp char */
  uint64_t           val = num; /* full value */
  unsigned long long digval;    /* value of digit */

  p = outStr;
  firstdig = p; /* save pointer to first digit */
  do
  {
    digval = (uint64_t)(val % 10ULL);
    val /= 10ULL; /* get next digit */

    /* convert to ascii and store */
    if (digval > 9)
      *p++ = (char)(digval - 10 + 'a'); /* a letter */
    else
      *p++ = (char)(digval + '0'); /* a digit */
  } while (val > 0);

  /* We now have the digit of the number in the buffer, but in reverse
   order.  Thus we reverse them now. */
  *p-- = '\0'; /* terminate string; p points to last digit */
  do
  {
    temp = *p;
    *p = *firstdig;
    *firstdig = temp; /* swap *p and *firstdig */
    --p;
    ++firstdig;         /* advance to next two digits */
  } while (firstdig < p); /* repeat until halfway */
}
#endif

#if RESOURCE_STATS_STRING_ENABLED

class AllocatorStringBuilder
{
 public:
  AllocatorStringBuilder(ResourceAllocator* alloc) {}
  size_t      GetLength() const { return m_Data.size(); }
  const char* GetData() const { return m_Data.data(); }

  void Add(char ch) { m_Data.push_back(ch); }
  void Add(const char* pStr);
  void AddNewLine() { Add('\n'); }
  void AddNumber(uint32_t num);
  void AddNumber(uint64_t num);
  void AddBool(bool b) { Add(b ? "true" : "false"); }
  void AddNull() { Add("null"); }
  void AddString(const char* pStr);

 private:
  AllocatorVector<char> m_Data;
};

void AllocatorStringBuilder::Add(const char* pStr)
{
  const size_t strLen = strlen(pStr);
  if (strLen > 0)
  {
    const size_t oldCount = m_Data.size();
    m_Data.resize(oldCount + strLen);
    memcpy(m_Data.data() + oldCount, pStr, strLen);
  }
}

void AllocatorStringBuilder::AddNumber(uint32_t num)
{
  char buf[11];
  AllocatorUint32ToStr(buf, sizeof(buf), num);
  Add(buf);
}

void AllocatorStringBuilder::AddNumber(uint64_t num)
{
  char buf[21];
  AllocatorUint64ToStr(buf, sizeof(buf), num);
  Add(buf);
}

void AllocatorStringBuilder::AddString(const char* pStr)
{
  Add('"');
  const size_t strLen = strlen(pStr);
  for (size_t i = 0; i < strLen; ++i)
  {
    char ch = pStr[i];
    if (ch == '\'')
    {
      Add("\\\\");
    }
    else if (ch == '"')
    {
      Add("\\\"");
    }
    else if (ch >= 32)
    {
      Add(ch);
    }
    else
      switch (ch)
      {
        case '\n': Add("\\n"); break;
        case '\r': Add("\\r"); break;
        case '\t': Add("\\t"); break;
        default: ASSERT(0 && "Character not currently supported."); break;
      }
  }
  Add('"');
}

// Correspond to values of enum AllocatorSuballocationType.
static const char* RESOURCE_SUBALLOCATION_TYPE_NAMES[] = {
    "FREE", "UNKNOWN", "BUFFER", "IMAGE_UNKNOWN", "IMAGE_LINEAR", "IMAGE_OPTIMAL", "IMAGE_RTV_DSV", "UAV",
};

static void AllocatorPrintStatInfo(AllocatorStringBuilder& sb, const AllocatorStatInfo& stat)
{
  sb.Add("{ \"Allocations\": ");
  sb.AddNumber(stat.AllocationCount);
  sb.Add(", \"Suballocations\": ");
  sb.AddNumber(stat.SuballocationCount);
  sb.Add(", \"UnusedRanges\": ");
  sb.AddNumber(stat.UnusedRangeCount);
  sb.Add(", \"UsedBytes\": ");
  sb.AddNumber(stat.UsedBytes);
  sb.Add(", \"UnusedBytes\": ");
  sb.AddNumber(stat.UnusedBytes);
  sb.Add(", \"SuballocationSize\": { \"Min\": ");
  sb.AddNumber(stat.SuballocationSizeMin);
  sb.Add(", \"Avg\": ");
  sb.AddNumber(stat.SuballocationSizeAvg);
  sb.Add(", \"Max\": ");
  sb.AddNumber(stat.SuballocationSizeMax);
  sb.Add(" }, \"UnusedRangeSize\": { \"Min\": ");
  sb.AddNumber(stat.UnusedRangeSizeMin);
  sb.Add(", \"Avg\": ");
  sb.AddNumber(stat.UnusedRangeSizeAvg);
  sb.Add(", \"Max\": ");
  sb.AddNumber(stat.UnusedRangeSizeMax);
  sb.Add(" } }");
}

#endif    // #if RESOURCE_STATS_STRING_ENABLED

static void InitStatInfo(AllocatorStatInfo& outInfo) {
  memset(&outInfo, 0, sizeof(outInfo));
  outInfo.SuballocationSizeMin = UINT64_MAX;
  outInfo.UnusedRangeSizeMin = UINT64_MAX;
}

static void CalcAllocationStatInfo(AllocatorStatInfo& outInfo, const AllocatorBlock& alloc) {
  outInfo.AllocationCount = 1;

  const uint32_t rangeCount = (uint32_t) alloc.m_Suballocations.size();
  outInfo.SuballocationCount = rangeCount - alloc.m_FreeCount;
  outInfo.UnusedRangeCount = alloc.m_FreeCount;

  outInfo.UnusedBytes = alloc.m_SumFreeSize;
  outInfo.UsedBytes = alloc.m_Size - outInfo.UnusedBytes;

  outInfo.SuballocationSizeMin = UINT64_MAX;
  outInfo.SuballocationSizeMax = 0;
  outInfo.UnusedRangeSizeMin = UINT64_MAX;
  outInfo.UnusedRangeSizeMax = 0;

  for (AllocatorSuballocationList::const_iterator suballocItem = alloc.m_Suballocations.cbegin();
       suballocItem != alloc.m_Suballocations.cend(); ++suballocItem) {
    const AllocatorSuballocation& suballoc = *suballocItem;
    if (suballoc.type != RESOURCE_SUBALLOCATION_TYPE_FREE) {
      outInfo.SuballocationSizeMin = RESOURCE_MIN(outInfo.SuballocationSizeMin, suballoc.size);
      outInfo.SuballocationSizeMax = RESOURCE_MAX(outInfo.SuballocationSizeMax, suballoc.size);
    } else {
      outInfo.UnusedRangeSizeMin = RESOURCE_MIN(outInfo.UnusedRangeSizeMin, suballoc.size);
      outInfo.UnusedRangeSizeMax = RESOURCE_MAX(outInfo.UnusedRangeSizeMax, suballoc.size);
    }
  }
}

// Adds statistics srcInfo into inoutInfo, like: inoutInfo += srcInfo.
static void AllocatorAddStatInfo(AllocatorStatInfo& inoutInfo, const AllocatorStatInfo& srcInfo) {
  inoutInfo.AllocationCount += srcInfo.AllocationCount;
  inoutInfo.SuballocationCount += srcInfo.SuballocationCount;
  inoutInfo.UnusedRangeCount += srcInfo.UnusedRangeCount;
  inoutInfo.UsedBytes += srcInfo.UsedBytes;
  inoutInfo.UnusedBytes += srcInfo.UnusedBytes;
  inoutInfo.SuballocationSizeMin = RESOURCE_MIN(inoutInfo.SuballocationSizeMin, srcInfo.SuballocationSizeMin);
  inoutInfo.SuballocationSizeMax = RESOURCE_MAX(inoutInfo.SuballocationSizeMax, srcInfo.SuballocationSizeMax);
  inoutInfo.UnusedRangeSizeMin = RESOURCE_MIN(inoutInfo.UnusedRangeSizeMin, srcInfo.UnusedRangeSizeMin);
  inoutInfo.UnusedRangeSizeMax = RESOURCE_MAX(inoutInfo.UnusedRangeSizeMax, srcInfo.UnusedRangeSizeMax);
}

static void AllocatorPostprocessCalcStatInfo(AllocatorStatInfo& inoutInfo) {
  inoutInfo.SuballocationSizeAvg =
      (inoutInfo.SuballocationCount > 0) ? AllocatorRoundDiv<uint64_t>(inoutInfo.UsedBytes,
                                                                       inoutInfo.SuballocationCount) : 0;
  inoutInfo.UnusedRangeSizeAvg =
      (inoutInfo.UnusedRangeCount > 0) ? AllocatorRoundDiv<uint64_t>(inoutInfo.UnusedBytes, inoutInfo.UnusedRangeCount)
                                       : 0;
}


