#ifndef STACK_PREDICATES
#define STACK_PREDICATES


/*@
// Represents a stack that grows down (cf. RP2040 stack)
predicate stack_p(StackType_t * pxStack, 
                    uint32_t ulStackDepth, 
                    StackType_t * pxTopOfStack, 
                    uint32_t ulFreeBytes,
                    uint32_t ulUsedCells,
                    uint32_t ulUnalignedBytes) =
    malloc_block_chars((char*) pxStack, ulStackDepth * sizeof(StackType_t)) &*&
    // Free stack cells. The size of this memory block is not necessarily a 
    // multiple of sizeof(StackType_t), due to bitvector arithmetic.
    // At least, we cannot prove it.
    chars((char*) pxStack, ulFreeBytes, _) &*&
    //integer_(pxTopOfStack + sizeof(StackType_t), sizeof(StackType_t), false, _) &*&;

    // If there is any free memory left in this stack,
    // pxTopOfStack points to the last sizeof(StackType_t) number of bytes.
    (char*) pxStack + ulFreeBytes == (char*) pxTopOfStack + sizeof(StackType_t) &*&
    // Used stack cells
    integers_(pxTopOfStack + 1, sizeof(StackType_t), false, ulUsedCells, _) &*&
    // Unaligned rest
    unalignedRestOfStack_p((char*) pxStack + ulFreeBytes + sizeof(StackType_t) * ulUsedCells, 
                           ulUnalignedBytes) &*&
    // `taskCHECK_FOR_STACK_OVERFLOW` macro on RP2040 port expects minimal stack size
    ulFreeBytes >= 0 &*&
    ulUsedCells >= 0 &*&
    ulFreeBytes + ulUsedCells * sizeof(StackType_t) >= 4 * sizeof(StackType_t);

predicate unalignedRestOfStack_p(char* p, uint32_t ulUnalignedBytes) =
    chars(p, ulUnalignedBytes, _);
@*/



#endif /* STACK_PREDICATES */