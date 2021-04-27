/*
Ivan Stus - 821201908
Kenny Kieu - 819180993
CS 570-05
Spring 2021
Assignment #3 - Page Table
Description - Map logical addresses to a physical page table data structure
*/

typedef struct PAGETABLE {
    // number of frames in page table
    int frames;
    //  number of bits in address;
    int numBits;    //WORK ON
    // number of Levels in the page table;
    int numLevels;
    // array of number of bits per level; //unsigned numberOfBits[numOfLevels]
    unsigned int *numBitsPerLevel;
    // array of bitMasks per level; //bitMasks[numOfLevels]
    unsigned int *bitMasks;
    // array of number of bits to shift to get page number for each level;
    unsigned int *bitShift;
    // array of number of entries per level page table; //EntryCount[level];
    int *EntryCount;
    // a pointer to the root level node;
    struct LEVEL *rootLevel;    
} PAGETABLE;

typedef struct LEVEL {
    int leafNode;   //If leaf node is true = 1
    int depth; //which level this is
    struct MAP *Map;    //Point to map
    struct PAGETABLE *PageTable; //point to the pagetable that contains the root level node WORK ON
    struct LEVEL **NextLevelPtr; //array of pages in this level, each element points to a level node in the next level WORK ON
} LEVEL;

typedef struct MAP {
    int valid;  //if valid address, set to 1
    int frames; //keeps track of map frames
} MAP;

//Function Declarations
void setPageTableBytesUsed(long int value);

long int getPageTableBytesUsed();

LEVEL *setLevel(PAGETABLE *PageTable, LEVEL *Level, int depth, int levelCount);

MAP *PageLookup(PAGETABLE *PageTable, unsigned int LogicalAddress);

unsigned int LogicalToPage(unsigned int LogicalAddress, unsigned int Mask, unsigned int Shift);

void PageInsert (PAGETABLE *PageTable, unsigned int LogicalAddress, int Frame);

void PageInsertII (LEVEL *Level, unsigned int LogicalAddress, int Frame);

unsigned int extract(unsigned int value, int position, int numBits);

int mask(int bits);