# EXPERIMENTS

The following experiments were conducted to analyze and compare the behavior of different memory allocation strategies (first-fit, best-fit, and worst-fit) under various allocation patterns. Each test file was designed to test specific scenarios to understand how the different strategies handle memory, allocation efficiency, and overall performance.

## test1.txt - 4096 bytes

Test1.txt, using 4096 bytes of memory space, shows how first-fit and best-fit strategies produce identical results in this case, while worst-fit differs significantly. First-fit and best-fit both place the allocations (216 and 716 bytes) in the middle of memory, keeping larger free blocks intact. On the other hand, worst-fit places the larger allocation (716 bytes) at the start of memory and the smaller one (216 bytes) later, resulting in more fragmented free space. Despite these different approaches, all strategies maintain the same total allocated (932 bytes) and free space (3164 bytes).

### first-fit summary
SUMMARY:
4 ALLOCATION CHUNKS (after merging adjacent free blocks):
chunk 0 location 0:616 bytes - free
chunk 1 location 616:216 bytes - allocated
chunk 2 location 832:716 bytes - allocated
chunk 3 location 1548:2548 bytes - free
ALLOCATED/FREE BYTES:
932 bytes allocated
3164 bytes free (merged)
Total overhead: 64 bytes (16 bytes per allocation)

### best-fit summary
SUMMARY:
4 ALLOCATION CHUNKS (after merging adjacent free blocks):
chunk 0 location 0:616 bytes - free
chunk 1 location 616:216 bytes - allocated
chunk 2 location 832:716 bytes - allocated
chunk 3 location 1548:2548 bytes - free
ALLOCATED/FREE BYTES:
932 bytes allocated
3164 bytes free (merged)
Total overhead: 64 bytes (16 bytes per allocation)

### worst-fit summary
SUMMARY:
4 ALLOCATION CHUNKS (after merging adjacent free blocks):
chunk 0 location 0:716 bytes - allocated
chunk 1 location 716:1732 bytes - free
chunk 2 location 2448:216 bytes - allocated
chunk 3 location 2664:1432 bytes - free
ALLOCATED/FREE BYTES:
932 bytes allocated
3164 bytes free (merged)
Total overhead: 64 bytes (16 bytes per allocation)


## test2.txt - 2000 bytes

test2.txt shows that all three allocation strategies (first-fit, best-fit, and worst-fit) produce identical results when working with 2000 bytes of memory. The test allocates 200, 500, and 1000 bytes sequentially, then frees all allocations before requesting a final 300-byte allocation. In all cases, the final state shows just two chunks: a 316-byte allocated chunk at the start (300 bytes plus 16 bytes overhead) and a 1684-byte free chunk. This identical behavior occurs because after freeing all allocations and merging free spaces, there's only one large free block available, which gives all strategies the same option for the final 300-byte allocation.

### first-fit summary
SUMMARY:
2 ALLOCATION CHUNKS (after merging adjacent free blocks):
chunk 0 location 0:316 bytes - allocated
chunk 1 location 316:1684 bytes - free
ALLOCATED/FREE BYTES:
316 bytes allocated
1684 bytes free (merged)
Total overhead: 32 bytes (16 bytes per allocation)

### best-fit summary
SUMMARY:
2 ALLOCATION CHUNKS (after merging adjacent free blocks):
chunk 0 location 0:316 bytes - allocated
chunk 1 location 316:1684 bytes - free
ALLOCATED/FREE BYTES:
316 bytes allocated
1684 bytes free (merged)
Total overhead: 32 bytes (16 bytes per allocation)

### worst-fit summary
SUMMARY:
2 ALLOCATION CHUNKS (after merging adjacent free blocks):
chunk 0 location 0:316 bytes - allocated
chunk 1 location 316:1684 bytes - free
ALLOCATED/FREE BYTES:
316 bytes allocated
1684 bytes free (merged)
Total overhead: 32 bytes (16 bytes per allocation)


## test3.txt - 4096 bytes

test3.txt was designed to demonstrate differences between first-fit and best-fit strategies by creating multiple allocations of varying sizes (500, 50, 370, 50, and 50 bytes) with some deallocations/freeing in between. However, both strategies produced identical memory layouts, with 5 allocated chunks and 3 free spaces (10, 616, and 2370 bytes). This is occurring because merging adjacent free blocks at every free, so often times we will wind up consolidating multiple smaller free fragments into one large block. The worst-fit strategy, however, showed a different pattern by placing the 386-byte allocation at the end of memory rather than in the middle, resulting in a different distribution of free spaces (396, 616, and 1984 bytes), though maintaining the same total allocated and free space as the other strategies.

### first-fit summary
SUMMARY:
8 ALLOCATION CHUNKS (after merging adjacent free blocks):
chunk 0 location 0:516 bytes - allocated
chunk 1 location 516:66 bytes - allocated
chunk 2 location 582:386 bytes - allocated
chunk 3 location 968:10 bytes - free
chunk 4 location 978:66 bytes - allocated
chunk 5 location 1044:616 bytes - free
chunk 6 location 1660:66 bytes - allocated
chunk 7 location 1726:2370 bytes - free
ALLOCATED/FREE BYTES:
1100 bytes allocated
2996 bytes free (merged)
Total overhead: 128 bytes (16 bytes per allocation)

### best-fit summary
SUMMARY:
8 ALLOCATION CHUNKS (after merging adjacent free blocks):
chunk 0 location 0:516 bytes - allocated
chunk 1 location 516:66 bytes - allocated
chunk 2 location 582:386 bytes - allocated
chunk 3 location 968:10 bytes - free
chunk 4 location 978:66 bytes - allocated
chunk 5 location 1044:616 bytes - free
chunk 6 location 1660:66 bytes - allocated
chunk 7 location 1726:2370 bytes - free
ALLOCATED/FREE BYTES:
1100 bytes allocated
2996 bytes free (merged)
Total overhead: 128 bytes (16 bytes per allocation)

### worst-fit summary
SUMMARY:
-------------------------------------------------
8 ALLOCATION CHUNKS (after merging adjacent free blocks):
chunk 0 location 0:516 bytes - allocated
chunk 1 location 516:66 bytes - allocated
chunk 2 location 582:396 bytes - free
chunk 3 location 978:66 bytes - allocated
chunk 4 location 1044:616 bytes - free
chunk 5 location 1660:66 bytes - allocated
chunk 6 location 1726:386 bytes - allocated
chunk 7 location 2112:1984 bytes - free
ALLOCATED/FREE BYTES:
1100 bytes allocated
2996 bytes free (merged)
Total overhead: 128 bytes (16 bytes per allocation)
