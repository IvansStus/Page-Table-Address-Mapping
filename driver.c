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
#include <pthread.h>
#include <sys/stat.h>
#include <sched.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include "byutr.h"
#include "output_mode_helpers.h"
#include "structFunctions.h"


int main(int argc, char **argv) {

    //Local variables
    long int pageTableBytesUsed;    //Tracks bytes allocated by page table
    int offset = 32;        //initialize to 32 for 32-bit address
    int nFlag = 0;          //"-n" option
    char *mode = NULL;      //holds next word after "-o"
    int memRef;             //holds memory count to be referenced
    char idx;               //holds 1st argument after getopt()
    char Option;            
    int fileNameCount = 0;  //holds filename from parsing command line  

    while ((Option = getopt(argc, argv, "n:o:")) != -1) {   //Parse command line   
        switch (Option) {        
        case 'n': //Number of addresses to process(memref)
        nFlag = 1;  //"-n" seen
        memRef = atoi(optarg);  //save process number to memref               
        break;

        case 'o':   //sets mode to argument after "-o"
        mode = optarg;
        break;

        default:
        exit(0);
        }
    }

    //Allocating memory for page table variables 
    PAGETABLE *PageTable;
    PageTable = malloc(sizeof(PAGETABLE));
    PageTable->numBits = 32;        //Set default value of number of bits in address to 32
    offset = PageTable->numBits;        
    PageTable->numBitsPerLevel = (unsigned int*)malloc(sizeof(unsigned int));
    PageTable->bitMasks = (unsigned int*)malloc(sizeof(unsigned int));
    PageTable->bitShift = (unsigned int*)malloc(sizeof(unsigned int));
    PageTable->bitShift[0] = PageTable->numBits;
    PageTable->EntryCount = (int*)malloc(sizeof(int));

    //Finding number of bits per level
    int count = 0;     
    for (idx = optind; idx < argc; idx++) {
        if (fileNameCount == 0) {   //Skips argument that includes filename
            fileNameCount++;
        }
        else {
            PageTable->numBitsPerLevel[count] = atoi(argv[idx]);    //Number of bits per level saved as next argument
            offset = offset - atoi(argv[idx]);      //Compute offset for given level
            count++;    //level count            
        }
    }
    PageTable->numLevels = count;   //After for loop, set number of levels to count

    //Initializing Page Table
    int i = 0;
    for (i = 0; i < PageTable->numLevels; i++) {
        PageTable->bitShift[i] -= PageTable->numBitsPerLevel[i];    //bitshift array set to number of bits per level
        PageTable->bitMasks[i] = mask(PageTable->numBitsPerLevel[i]) << PageTable->bitShift[i]; //Calculate bitmask 
        PageTable->EntryCount[i] = pow(2, PageTable->numBitsPerLevel[i]);   //calculate entry count using number of bits per level
        PageTable->bitShift[i + 1] = PageTable->bitShift[i];
    }

    //Initialize Level
    PageTable->rootLevel = setLevel(PageTable, PageTable->rootLevel, 0, count);
    pageTableBytesUsed = 0;
    pageTableBytesUsed += sizeof(PageTable);
    
    FILE *fp = fopen("trace.sample.tr", "r");    //Open file
    if (fp == NULL) {
        printf("\nFile could not open\n");
        exit (0);        
    }       
    p2AddrTr trace_item;    //holds logical address from trace file 
    unsigned int addressHold;   //logical address variable
    PageTable->frames = 0;  //set frames to 0
    int hit = 0;
    int miss = 0;
    int processesAddressed = memRef;
    while (!feof(fp)) {     //Until EOF     
        if (nFlag == 1 && memRef > 0) {     //"-n" seen, if process count reaches 0, end loop
            memRef--;
        }
        else if (memRef == 0) { 
            break;
        }
        NextAddress(fp, &trace_item);   //Gather logical address            
        addressHold = trace_item.addr;  
        if (PageLookup(PageTable, addressHold) == NULL) {   //If invalid entry, report miss and insert page
            miss++;
            PageInsert(PageTable, addressHold, PageTable->frames++);
        }
        else {
            hit++;
        }

        //Output Helpers
        if (strcmp(mode, "offset") == 0) {     
            unsigned int newOffset = addressHold;   //output helper offset
            newOffset &= ((1 << offset) - 1);   //Calculate offset    
            report_logical2offset(addressHold, newOffset);
        }
        if (strcmp(mode, "logical2physical") == 0) {
            MAP *tempMap = PageLookup(PageTable, addressHold);             
            report_logical2physical(addressHold, ((((tempMap->frames) << offset) & ~mask(offset)) | (addressHold & mask(offset))));
        }
        if (strcmp(mode, "page2frame") == 0) {     
            int j = 0;
            int tempOffset = 32;
            MAP *tempMap = PageLookup(PageTable, addressHold);
            unsigned int *page;
            page = (unsigned int *)malloc(sizeof(unsigned int));
            for (j = 0; j < count; j++) {   //Set each bit from logical address to correct array value for printing
                tempOffset -= PageTable->numBitsPerLevel[j];    //Offset changes between levels
                page[j] = extract(addressHold, tempOffset, PageTable->numBitsPerLevel[j]);                
            }
            report_pagemap(addressHold, PageTable->numLevels, page, tempMap->frames);            
        }           
    }
    if (strcmp(mode, "bitmasks") == 0) {           
        report_bitmasks(count, PageTable->bitMasks);
    }
    if (strcmp(mode, "summary") == 0) {
        pageTableBytesUsed += getPageTableBytesUsed();  //Call function from other .c file to gather bytes
        int pageSize = pow(2, offset);        
        report_summary(pageSize, hit, processesAddressed, PageTable->frames, pageTableBytesUsed);
    }
    return 0;
}


