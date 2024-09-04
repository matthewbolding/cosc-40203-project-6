/**
 * @file main.h
 * @author Matthew Bolding (matthew.bolding@tcu.edu)
 * @brief 
 * @version 0.1
 * @date 2022-04-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <argp.h>
#include <string.h>
#include <list>
#include <stdlib.h>

#include "definitions.h"

using namespace std;

// Page Table and TLB Data Structures
struct pt_entry_s *page_table;
list<tlb_entry_s> tlb;

// Character array to mimic physical memory
char *memory;

// Variables to manage the number of frames and which ones have been used.
long unsigned int frames;
list<unsigned char> free_frames;
list<unsigned char> fifo_frame;
list<unsigned char> lru_frame;

// Counter Variables
int page_faults = 0;
int tlb_hits = 0;
int refs = 0;

FILE *swap_file;
FILE *address_file;

// Prototypes for functions initializing the environment.
int initialize_environment(int argc, char **argv);
void initialize_free_frames(list<unsigned char> *list);
struct arguments get_arguments(int argc, char **argv);
FILE *get_file(const char *filename);

// Prototypes for functions handeling address file input.
unsigned short clean(int ingest);
unsigned char page_number(unsigned short logical_address);
unsigned char address_offset(unsigned short logical_address);

// Prototypes for functions handeling referencing memory.
bool search_tlb(unsigned char *page, unsigned char *frame, int *mode);
void update_tlb(unsigned char *page, unsigned char *frame);
bool search_page_table(unsigned char *page, unsigned char *frame, int *mode);
void page_fault(unsigned char *page, int *mode);