#include "debug_new_and_delete.hpp"

#include <exception> // for std::bad_alloc
#include <new>
#include <cstdlib> // for malloc() and free()
#include <stdint.h>
#include <iostream>

/*
This overrides the global operator new and delete. The overrided new allocates
extra data before and after the new memory as markers to check for buffer
overflows in either direction. New and delete also track the number of bytes
allocated here, and checks for multiple deletes of the same pointer.

Blocks smaller than 256 bytes are never freed. The first million of these small
blocks will remain in an array which will be scanned when errorsInFreedMem() is
called. The scanning checks that the data has not been written to since it was
deleted.
*/

struct Suffix {
	uint32_t checkValue; // Must be 0x9ABC for active memory, and 0x99BC for freed memory
};

struct Prefix {
	uint32_t checkValue1; // Must be 0xABCD for active memory, and 0xAACD for freed memory
	Prefix* prev;
	Prefix* next;
	Suffix* suffix;
	size_t len;
	uint32_t checkValue2; // Must be 0xABCD for active memory, and 0xAACD for freed memory
};

size_t bytesAllocatedCount = 0;
bool isMemoryCorrputed = false;
bool isMultipleFree = false;

Prefix* firstDeallocated = 0;
Prefix* lastDeallocated = 0;
Prefix* firstAllocated = 0;
Prefix* lastAllocated = 0;

int checkConsistency() {
    if (isMemoryCorrputed)
        return 1;

	// Look through allocated blocks
	Prefix* block = firstAllocated;
	while (block)
	{
		//std::cout << (block+1) << std::endl;
		//std::cout << block->checkValue1 << " " << block->checkValue2 << " " << block->suffix->checkValue << std::endl;
		if (block->checkValue1 != 0xABCD || block->checkValue2 != 0xABCD || block->suffix->checkValue != 0x9ABC)
		{
			isMemoryCorrputed = true;
			return 1;
		}
		block = block->next;
	}

	block = firstDeallocated;
	while (block)
	{
		if (block->checkValue1 != 0xAACD || block->checkValue2 != 0xAACD || block->suffix->checkValue != 0x99BC)
		{
			isMemoryCorrputed = true;
			return 2;
		}
		uint32_t* p = (uint32_t*)(block + 1);
		size_t count = block->len/sizeof(uint32_t*);
		while (count--)
		{
			if (*(p++) != 0xFEEEFEEE)
				return 3;
		}
		block = block->next;
	}

	return 0;
}


void* myAlloc(size_t size) throw (std::bad_alloc) {

	size_t space = size;// | 0x03; // Round up to nearest 4 bytes (may need to change this)

	Prefix* block = (Prefix*)malloc(sizeof(Prefix) + space + sizeof(Suffix));
	block->checkValue1 = 0xABCD;
	block->checkValue2 = 0xABCD;
	block->len = size;
	block->suffix = (Suffix*)((uint8_t*)block + sizeof(Prefix) + space);
	block->suffix->checkValue = 0x9ABC;

	block->next = 0;
	block->prev = lastAllocated;
	if (!lastAllocated)
		firstAllocated = block;
	lastAllocated = block;

    bytesAllocatedCount += size;

    return (block + 1);
}

void* operator new (size_t size) throw (std::bad_alloc) {
    return myAlloc(size);
}

void* operator new[]   ( size_t size ) { 
    return myAlloc( size ); 
}

void myFree (void *p_) throw() {
    if (p_ == 0)
        return;

    Prefix* block = (Prefix*)p_ - 1;

    try
    {
        if ((block->checkValue1 == 0xAACD) || (block->checkValue2 == 0xAACD)) {
            std::cerr << "********************************************************************************" << std::endl;
            std::cerr << "**********************! Probably freeing same memory twice !********************" << std::endl;
            std::cerr << "********************************************************************************" << std::endl;
            isMultipleFree = true;
        }
        else if ((block->checkValue1 != 0xABCD) || (block->checkValue2 != 0xABCD) || (block->suffix->checkValue != 0x9ABC)) {
            std::cerr << "********************************************************************************" << std::endl;
            std::cerr << "*****************************! Memory corrupted !*******************************" << std::endl;
            std::cerr << "********************************************************************************" << std::endl;
            isMemoryCorrputed = true;
        }
        else {
            bytesAllocatedCount -= block->len;

            // Mark as freed
            block->checkValue1 = 0xAACD;
            block->checkValue2 = 0xAACD;
            block->suffix->checkValue = 0x99BC;

            // Remove from allocated linked list
            if (block->next)
                block->next->prev = block->prev;
            if (block->prev)
                block->prev->next = block->next;
            if (firstAllocated == block)
            	firstAllocated = block->next;
            if (lastAllocated == block)
            	lastAllocated = block->prev;
            block->prev = 0;
            block->next = 0;

            // Large blocks can actually be freed.
            if (block->len > 256)
                free(block);
            else {
            	// Smaller blocks we keep around to make sure they don't get used after they're freed
            	size_t count = block->len / sizeof(uint32_t);
            	uint32_t* data = (uint32_t*)p_;
                for (uint32_t b = 0; b < count; b ++) {
                    data[b] = 0xFEEEFEEE;
                }

                if (firstDeallocated == 0)
                	firstDeallocated = block;
                block->prev = lastDeallocated;
                block->next = 0;
                lastDeallocated = block;
            }
        }
    }
    catch (...)
    {
    }
}

void operator delete (void *p_) throw() {
    myFree(p_);
}

void operator delete[] (void *p_) throw() {
    myFree(p_);
}