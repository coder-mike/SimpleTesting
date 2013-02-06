#ifndef DEBUG_NEW_AND_DELETE_HPP_INCLUDED
#define DEBUG_NEW_AND_DELETE_HPP_INCLUDED

#include <cstring>

extern size_t bytesAllocatedCount;
extern bool isMemoryCorrputed;
extern bool isMultipleFree;
int checkConsistency(); // 0 = OK, 1 = Heap corrupt, 2 = Free header corrupt, 3 = Free memory used

#endif // DEBUG_NEW_AND_DELETE_HPP_INCLUDED
