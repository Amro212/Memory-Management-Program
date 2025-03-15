#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "mmanager.h"

#define MAX_CHUNKS 100 // maximum number of chunks (fixed array size)
#define BLOCK_OVERHEAD sizeof(block)   // overhead size for each memory block (size of 'block' struct)

// structure to store chunk information
typedef struct {
	int id;
	int start;
	int size;
	char paint;
	bool allocated; // whether the block is allocated
} block;

static block chunks[MAX_CHUNKS]; // array to store chunk information
static int chunk_count = 0;// number of chunks in the array

// checks if we can merge free memory blocks that are next to each other
static void merge_adjacent_free_blocks() {
    int i = 0;
    while (i < chunk_count - 1) {
        if (!chunks[i].allocated && !chunks[i + 1].allocated) {
            // merge the next chunk into the current one
            chunks[i].size += chunks[i + 1].size;
            
            // shift all remaining chunks left
            for (int j = i + 1; j < chunk_count - 1; j++) {
                chunks[j] = chunks[j + 1];
            }
            chunk_count--;
        } else {
            i++;
        }
    }
}

int runModel(FILE *outputfp, FILE *inputfp,
		long totalMemorySize, int fitStrategy,
		int verbosity)
{
	char *memoryBlock = NULL;
	int nSuccessfulActions = 0;
	mmgr_action action;

	fprintf(outputfp,
			"Running a %s-fit model in %ld (0x%lx) bytes of memory.\n",
			(fitStrategy == STRAT_FIRST ? "first" :
				fitStrategy == STRAT_BEST ? "best" :
					fitStrategy == STRAT_WORST ? "worst" : "unknown"),
			totalMemorySize, totalMemorySize);

	/**
	 * this is the only allocation you should need -- all requests
	 * from your model should come from this allocated block
	 */
	memoryBlock = (char *) malloc(totalMemorySize);
	if (memoryBlock == NULL) {
		perror("allocation failed!");
		return -1;
	}

	/**
	 *	+++ Set up anything else you will need for your memory management
	 */
	memset(chunks, 0, sizeof(chunks));
	chunks[0].start = 0;
	chunks[0].size = totalMemorySize;
	chunks[0].allocated = false;
	chunk_count = 1;
	

	while (getAction(&action, inputfp, outputfp, verbosity) > 0)
	{
		if (action.type == ACTION_ALLOCATE)
		{
			int allocation_result = -1;
			int total_size = action.size + BLOCK_OVERHEAD;  // Add overhead to requested size
			
			// handling different fit strategies
			switch(fitStrategy) {
				// first-fit implementation
				case STRAT_FIRST:
					for (int i = 0; i < chunk_count; i++) {
						if (!chunks[i].allocated && chunks[i].size >= total_size) {  // Check against total_size
							chunks[i].allocated = true;
							chunks[i].id = action.id;

							if(chunks[i].size > total_size) {
								// move all chunks one position forward 
								for(int j = chunk_count; j > i + 1; j--) {
									chunks[j] = chunks[j-1];
								}
								// create the new free chunk
								chunks[i+1].start = chunks[i].start + total_size;
								chunks[i+1].size = chunks[i].size - total_size;
								chunks[i+1].allocated = false;
								chunks[i].size = total_size;
								chunk_count++;
							}
							// Store block metadata at start of allocation
							block* metadata = (block*)(memoryBlock + chunks[i].start);
							*metadata = chunks[i];
							allocation_result = chunks[i].start + BLOCK_OVERHEAD;
							break;
						}
					}
					break;

				case STRAT_BEST: {
                // best-fit strategy
                int best_index = -1;
                int best_size = totalMemorySize + 1; // a guaranteed bigger number than totalMemorySize

                for (int i = 0; i < chunk_count; i++) {
                    if (!chunks[i].allocated && chunks[i].size >= total_size) {
                        if (chunks[i].size < best_size) {
                            best_index = i;
                            best_size = chunks[i].size;
                        }
                    }
                }

                if (best_index != -1) {
                    chunks[best_index].allocated = true;
                    chunks[best_index].id = action.id;

                    if (chunks[best_index].size > total_size) {
                        for (int j = chunk_count; j > best_index + 1; j--) {
                            chunks[j] = chunks[j - 1];
                        }
                        chunks[best_index + 1].start = chunks[best_index].start + total_size;
                        chunks[best_index + 1].size = chunks[best_index].size - total_size;
                        chunks[best_index + 1].allocated = false;
                        chunks[best_index].size = total_size;
                        chunk_count++;
                    }
                    // Store block metadata at start of allocation
                    block* metadata = (block*)(memoryBlock + chunks[best_index].start);
                    *metadata = chunks[best_index];
                    allocation_result = chunks[best_index].start + BLOCK_OVERHEAD;
                }
                break;
            }

				// worst-fit strategy
				case STRAT_WORST: {
                    int worst_index = -1;
                    int worst_size = -1; // to track size of largest free block found so far

                    for (int i = 0; i < chunk_count; i++) {
                        if (!chunks[i].allocated && chunks[i].size >= total_size) {
                            if (chunks[i].size > worst_size) {
                                worst_index = i;
                                worst_size = chunks[i].size;
                            }
                        }
                    }

                    if (worst_index != -1) {
                        chunks[worst_index].allocated = true;
                        chunks[worst_index].id = action.id;

                        if (chunks[worst_index].size > total_size) {
                            for (int j = chunk_count; j > worst_index + 1; j--) {
                                chunks[j] = chunks[j - 1];
                            }
                            chunks[worst_index + 1].start = chunks[worst_index].start + total_size;
                            chunks[worst_index + 1].size = chunks[worst_index].size - total_size;
                            chunks[worst_index + 1].allocated = false;
                            chunks[worst_index].size = total_size;
                            chunk_count++;
                        }
                        // Store block metadata at start of allocation
                        block* metadata = (block*)(memoryBlock + chunks[worst_index].start);
                        *metadata = chunks[worst_index];
                        allocation_result = chunks[worst_index].start + BLOCK_OVERHEAD;
                    }
                    break;
                }

			}
			
			if(allocation_result >= 0) {
				fprintf(outputfp, "alloc %d bytes: SUCCESS - return location %d\n", 
						action.size, allocation_result);
			} else {
				fprintf(outputfp, "alloc %d bytes: FAIL\n", action.size);
			}
		}
		else if(action.type == ACTION_RELEASE) {
			bool found = false;
			
			for(int i = 0; i < chunk_count; i++) {
				if(chunks[i].allocated && chunks[i].id == action.id) {
					chunks[i].allocated = false;
					found = true;
					fprintf(outputfp, "free location %lld\n", chunks[i].start + BLOCK_OVERHEAD);
					merge_adjacent_free_blocks();
					break;
				}
			}

			if(!found) {
				fprintf(outputfp, "free %d: FAIL (ID not found)\n", action.id);
			}
		}

		nSuccessfulActions++;
	}

	
	// SUMMARY SECTION
	fprintf(outputfp, "-------------------------------------------------");
	fprintf(outputfp, "\nSUMMARY:\n");
	fprintf(outputfp, "-------------------------------------------------");
	int totalAllocated = 0;
	int totalFree = 0;

	fprintf(outputfp, "\n%d ALLOCATION CHUNKS (after merging adjacent free blocks):\n", chunk_count);
	for(int i = 0; i < chunk_count; i++) {
		if(chunks[i].allocated) {
			totalAllocated += chunks[i].size;
		}
		else {
			totalFree += chunks[i].size;
		}

		fprintf(outputfp, "chunk %d location %d:%d bytes - %s\n",
                i, chunks[i].start, chunks[i].size,
                chunks[i].allocated ? "allocated" : "free");
	}

	
	fprintf(outputfp, "ALLOCATED/FREE BYTES:\n");
	fprintf(outputfp, "%d bytes allocated\n", totalAllocated);
	fprintf(outputfp, "%d bytes free (merged)\n", totalFree);
	fprintf(outputfp, "Total overhead: %lld bytes (%llu bytes per allocation)\n\n", 
        BLOCK_OVERHEAD*chunk_count, BLOCK_OVERHEAD);
	
	
	free(memoryBlock);
	return nSuccessfulActions;
}