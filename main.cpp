/**
 * @file main.cpp
 * @author Matthew Bolding (matthew.bolding@tcu.edu)
 * @brief 
 * @version 0.1
 * @date 2022-04-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

using namespace std;

#include "main.h"
#include <errno.h>
#include <algorithm>

int main(int argc, char **argv) {
    int mode = initialize_environment(argc, argv);
    
    char value;
    char ingest[INGEST_SIZE];
    
    unsigned short raw;
    unsigned char page, offset, frame;

    while (fgets(ingest, INGEST_SIZE, address_file) != NULL) {
        refs++;
        raw = clean(atoi(ingest));

        // Get the page and offset.
        page = page_number(raw);
        offset = address_offset(raw);

        // Follow reference algorithm.
        // If there's not a TLB hit,
        if(!search_tlb(&page, &frame, &mode)) {
            // search the page table. If there page's frame is invalid,
            FAULT:if(!search_page_table(&page, &frame, &mode)) {
                // handle the page fault.
                page_faults++;
                page_fault(&page, &mode);
                goto FAULT;
            }
            // Update the TLB, since the entry is not in the table.
            update_tlb(&page, &frame);
        }

        // Calculate the physical address and reference.
        unsigned short phys = (frame << 8) | offset;
        value = memory[phys];

        printf("Virtual address: %d Physical address: %d Value: %d\n", raw, phys, value);
    }

    // Print statistics.
    printf("Number of Translated Addresses = %d\n", refs);
    printf("Page Faults = %d\n", page_faults);
    printf("Page Fault Rate = %.3f\n", ((double) page_faults / (double) refs));
    printf("TLB Hits = %d\n", tlb_hits);
    printf("TLB Hit Rate = %.3f\n", ((double) tlb_hits / (double) refs));

    // Free objects allocated with calloc().
    fclose(swap_file);
    fclose(address_file);
    free(page_table);
    free(memory);

    return 0;
}

/**
 * @brief Get the file object for a specific file.
 * 
 * @param filename the file name
 * @return FILE*, the file object, opened as read only
 */
FILE *get_file(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "%s: ", filename);
        perror(NULL);
        exit(1);
    }

    return file;
}

/**
 * @brief Helper function to return only 16 least significant bits of an integer.
 * 
 * @param ingest the integer
 * @return unsigned short
 */
unsigned short clean(int ingest) {
    return ((unsigned short) (0x0000FFFF & ingest));
}

/**
 * @brief Helper function to return the 8 most significant bits of a short.
 * 
 * @param logical_address the short
 * @return unsigned char
 */
unsigned char page_number(unsigned short logical_address) {
    return (unsigned char) (logical_address >> 8);
}

/**
 * @brief Helper function to return the 8 least significant bits of a short.
 * 
 * @param logical_address the short 
 * @return unsigned char 
 */
unsigned char address_offset(unsigned short logical_address) {
    return (unsigned char) (0x000000FF & logical_address);
}

/**
 * @brief Function to determine if a page is loaded in physical memory.
 * 
 * @param page the page
 * @param frame the frame
 * @param mode the mode
 * @return true, if the page is loaded
 * @return false, if the page is not loaded
 */
bool search_page_table(unsigned char *page, unsigned char *frame, int *mode) {
    if(page_table[*page].valid) {
        // Get the frame number.
        *frame = page_table[*page].frame_number;

        // Push the frame to the back of the LRU list.
        if(*mode == LRU) {
            lru_frame.remove(*frame);
            lru_frame.push_back(*frame);
        }

        return true;
    }

    return false;
}

void page_fault(unsigned char *page, int *mode) {
    unsigned char frame;

    // If free frames still exist,
    if(free_frames.size() != 0) {
        // get the next one.
        frame = free_frames.front(); free_frames.pop_front();

        // Put these in the FIFO/LRU list.
        if(*mode == FIFO) fifo_frame.push_back(frame);
        else if(*mode == LRU) lru_frame.push_back(frame);
    } else {
        // We will never get to this part when mode is DEMAND,
        // since the entire backing store can be put in memory.

        if(*mode == FIFO) {
            frame = fifo_frame.front();
            fifo_frame.pop_front();
            fifo_frame.push_back(frame);
        } else if(*mode == LRU) {
            frame = lru_frame.front();
        }

        // Since a frame is getting replaced, find the page corresponding to the
        // frame to be overwritten and mark that entry as invalid.
        for(int i = 0; i < PAGE_TABLE_SIZE; i++) {
            if(page_table[i].frame_number == frame) {
                page_table[i].valid = false;
            }
        }
    }

    // Load the frame from backing store.
    fseek(swap_file, (*page) * FRAME_SIZE, SEEK_SET);
    fread(&memory[(frame * FRAME_SIZE)], sizeof(unsigned char), FRAME_SIZE, swap_file);

    // Update the page table entry.
    page_table[*page].frame_number = frame;
    page_table[*page].valid = true;
}

/**
 * @brief 
 * 
 * @param page the page
 * @param frame the frame
 * @param mode the mode
 * @return true 
 * @return false 
 */
bool search_tlb(unsigned char *page, unsigned char *frame, int *mode) {
    // Search for a hit, if an entry's page matches the requested one.
    for (auto const& entry : tlb) {
        if(entry.page == *page) {
            tlb_hits++;
            *frame = entry.frame;

            // Push the frame to the back of the LRU list.
            if(*mode == LRU) {
                lru_frame.remove(*frame);
                lru_frame.push_back(*frame);
            }

            return true;
        }
    }

    return false;
}

/**
 * @brief Helper function to update the TLB.
 * 
 * @param page the page
 * @param frame the frame
 */
void update_tlb(unsigned char *page, unsigned char *frame) {
    // We are using a FIFO approach!
    struct tlb_entry_s item;
    
    item.page = *page;
    item.frame = *frame;

    // If the table is full, pop top-most entry,
    if(tlb.size() == TLB_SIZE) tlb.pop_front();

    // and push the new one on.
    tlb.push_back(item);
}

/**
 * @brief Helper function to initialize the free-frame list.
 * 
 * @param list the list of free frames
 */
void initialize_free_frames(list<unsigned char> *list) {
    long unsigned int i;

    for(i = 0; i < frames; i++) {
        list->push_back((unsigned char) i);
    }
}

/**
 * @brief Helper function to initialize the environment.
 * 
 * @param argc the number of command line arguments
 * @param argv array of command line argument strings
 * @return int, expressing the mode
 */
int initialize_environment(int argc, char **argv) {
    struct arguments args = get_arguments(argc, argv);

    swap_file = get_file(args.swapfile);
    address_file = get_file(args.addressfile);

    // The number of frames depends on the mode!
    if(args.mode == DEMAND) frames = 256;
    else frames = 128;

    // Allocate memory for the page table and physical memory.
    page_table = (struct pt_entry_s *) calloc(PAGE_TABLE_SIZE, sizeof(struct pt_entry_s));
    memory = (char *) calloc(frames * FRAME_SIZE, sizeof(char));
    
    initialize_free_frames(&free_frames);

    return args.mode;
}