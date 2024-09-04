# COSC 40203 Operating Systems
Virtual Memory Simulator
Due Friday, April 29th, 2022

## Overview

This project consists of writing a program that translates logical to physical addresses for a virtual address space of size 2<sup>16</sup> = 65536 bytes. The program will read from a file containing logical addresses and, using a TLB as well as a page table, will translate each logical address to its corresponding physical address and output the value of the byte stored at the translated physical address. The goal behind this project is to simulate the steps involved in translating logical to physical addresses.

## How to Run `vmsim`

* `-s` specifies the swap file or backing store file used for this simulation.
* `-a` specified the name of a file containing the addresses that are to be referenced from the backing store.
* `-m` specifies the case-sensitive mode, either `DEMAND`, `FIFO`, or `LRU`.
    + If the mode is `DEMAND`, then then number of frames will be 256. In this case, there is not need for a frame replacement strategy, since physical memory will be the same size as the provided backing store, `swapfile.bin`.
    + If the mode is `FIFO`, then the number of frames will be 128 and the frame replacement algorithm will implement a first-in, first out queue.
    + If the mode is `LRU`, then the number of frames will be 128 and the frame replacement algorithm will implement a least recently used queue.

## Swap and Address Files
Provided is the file `addresses.txt`, which contains integer values representing logical addresses ranging from 0 to 65535 (the size of the virtual address space). The program will open this file, read each logical address and translate it to its corresponding physical address, and output the value of the signed byte at the physical address.

The backing store is represented by the file swapfile.bin, a binary file of size 65,536 bytes. This file contains 1000 integers ranging from 0 to 65535.

## How to Compile
Run the `make` command to compile the excutable, and `make clean` to remove all `.o` files and the `vmsim` executable.

## Output
After completion, the program will report the following statistics:
* the page-fault rate, the percentage of address references that resulted in page faults,
* the TLB hit rate, the percentage of address references that were resolved in the TLB, and
* a decomposition of each virtual address to physical address and the value.