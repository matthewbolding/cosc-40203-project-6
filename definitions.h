/**
 * @file definitions.h
 * @author Matthew Bolding (matthew.bolding@tcu.edu)
 * @brief 
 * @version 0.1
 * @date 2022-04-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdbool.h>

#define DEMAND          1
#define FIFO            2
#define LRU             3

// Macros to define mode-independent variables.
#define INGEST_SIZE     8
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE      256
#define TLB_SIZE        16

/**
 * @brief struct to define a page table entry, including frame number and valid bit
 * 
 */
struct pt_entry_s {
    unsigned char frame_number;
    bool valid;
};

/**
 * @brief struct to define a translation-look-aside entry, including page and frame number
 * 
 */
struct tlb_entry_s {
    unsigned char page;
    unsigned char frame;
};

/**
 * @brief struct to define program arguments
 * 
 */
struct arguments {
    int mode;
    char *swapfile;
    char *addressfile;
};