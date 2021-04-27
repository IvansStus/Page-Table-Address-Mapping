/*
Ivan Stus - 821201908
Kenny Kieu - 819180993
CS 570-05
Spring 2021
Assignment #3 - Page Table
Description - Map logical addresses to a physical page table data structure
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "structFunctions.h"

long int pageTableBytesUsed;    //collects bytes allocated

void setPageTableBytesUsed(long int value) {    
    pageTableBytesUsed = value;
}

long int getPageTableBytesUsed() {
    return pageTableBytesUsed;
}

unsigned int extract(unsigned int value, int position, int numBits)   //Extracting bits from logical address to help with printing
{
    unsigned int mask = (1 << (numBits)) - 1; //Calculate bitmask
    return (value >> position) & mask;  //return bit specified
}

int mask(int bits) {    //bitmask
    return (1 << bits) - 1;
}

LEVEL *setLevel(PAGETABLE *PageTable, LEVEL *Level, int depth, int levelCount) {
    long int bytesUsed = 0;     //local variable for bytes used
    Level = malloc(sizeof(LEVEL));  //initialize level to refernce page table  
    Level->depth = depth;   
    Level->PageTable = PageTable;
    Level->leafNode = (depth + 1) >= levelCount;  
    if (Level->leafNode) {   //if leaf node exists
        Level->Map = malloc(PageTable->EntryCount[depth] * sizeof(MAP));    //initialize map to reference valid bit
        int i = 0;
        for (i = 0; i < PageTable->EntryCount[depth]; i++) {
            Level->Map[i].valid = 0;    //False value for valid
        }
        bytesUsed += sizeof(Level) + sizeof(LEVEL*) * PageTable->EntryCount[depth]; //Allocate bytes 
        setPageTableBytesUsed(bytesUsed);      
    }
    else {
        Level->NextLevelPtr = malloc(PageTable->EntryCount[depth] * sizeof(LEVEL*));    //Set next level ptr to next level
        bytesUsed += sizeof(LEVEL*) + sizeof(MAP*); //Allocate bytes
        setPageTableBytesUsed(bytesUsed);
    }
    return Level;
}

void PageInsert (PAGETABLE *PageTable, unsigned int LogicalAddress, int Frame) {
    return PageInsertII(PageTable->rootLevel, LogicalAddress, Frame);   //Transfer root level to pageInsertII
}

void PageInsertII (LEVEL *Level, unsigned int LogicalAddress, int Frame) {
    unsigned int address = LogicalToPage(LogicalAddress, Level->PageTable->bitMasks[Level->depth], Level->PageTable->bitShift[Level->depth]);   
    if (Level->leafNode) {  //If leaf node exists
        Level->Map[address].valid = 1;  //make address valid
        Level->Map[address].frames = Frame; //Save to frame
    }
    else {
        if (Level->NextLevelPtr[address] == NULL) { //if next level ptr doesn't exist. create new level            
            Level->NextLevelPtr[address] = setLevel(Level->PageTable, Level, Level->depth+1, Level->PageTable->numLevels);       
        }
        PageInsertII(Level->NextLevelPtr[address], LogicalAddress, Frame); //Recursive method if leaf node doesn't exist
    }    
}

unsigned int LogicalToPage(unsigned int LogicalAddress, unsigned int Mask, unsigned int Shift) {
    //shift logical address          
    unsigned int bitWise = LogicalAddress & Mask;
    bitWise = bitWise >> Shift;
    return bitWise; //Return page number
}

MAP *PageLookup(PAGETABLE *PageTable, unsigned int LogicalAddress) {    
    LEVEL *level = PageTable->rootLevel;    //Initialize level struct to reference page table root level
    unsigned int address; 
    while (level) { //continue running until leaf node exists
        address = LogicalToPage(LogicalAddress, level->PageTable->bitMasks[level->depth], level->PageTable->bitShift[level->depth]);
        if (level->leafNode) {  //Break while loop when leaf node exists
            break;
        }
        if (level->NextLevelPtr[address] == NULL) { //Page doesn't exist
            return NULL;
        }
        level = level->NextLevelPtr[address];   
    }
    if (level->Map[address].valid) {    //When page is found
            return &level->Map[address];
    }
    else {  //When page is not found
        return NULL;
    } 
}