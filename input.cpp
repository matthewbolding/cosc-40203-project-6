/**
 * @file input.cpp
 * @author Matthew Bolding (matthew.bolding@tcu.edu)
 * @brief 
 * @version 0.1
 * @date 2022-04-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "input.h"

using namespace std;

bool verbose = false;

/**
 * @brief Getter function which returns an instance of the arguments
 * struct, containing all necessary information to run the program.
 * 
 * @param argc the number of command line arguments
 * @param argv array of command line argument strings
 * @return struct arguments 
 */
struct arguments get_arguments(int argc, char **argv) {
    int i;
    struct arguments args;

    if(argc != 7) {
        fprintf(stderr, "Usage: ./vmsim -s [SWAP FILE] -a [ADDRESS FILE] -m [MODE]\n");
        printf("\nFor more information, see README.md.\n");
        exit(1);
    }

    for(i = 1; i < argc; i = i + 2) {
        if(!strcmp(argv[i], "-s")) {
            args.swapfile = argv[i + 1];
        } 
        else if(!strcmp(argv[i], "-a")) {
            args.addressfile = argv[i + 1];
        } 
        else if(!strcmp(argv[i], "-m")) {
            if(string(argv[i + 1]) == "DEMAND") {
                args.mode = DEMAND;
            } else if(string(argv[i + 1]) == "FIFO") {
                args.mode = FIFO;
            } else if(string(argv[i + 1]) == "LRU") {
                args.mode = LRU;
            } else {
                fprintf(stderr, "Invalid mode!\n");
                printf("\nFor more information, see README.md.\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "Invalid parameter!\n");
            printf("\nFor more information, see README.md.\n");
            exit(1);
        }
    }

    if(verbose) {
        printf("Swapfile File: %s\n", args.swapfile);
        printf("Address File: %s\n", args.addressfile);
        printf("Mode: %d\n", args.mode);
    }

    return args;
}