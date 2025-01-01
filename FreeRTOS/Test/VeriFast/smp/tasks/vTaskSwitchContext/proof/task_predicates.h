#ifndef TASKS_GH

#define TASKS_GH

#include "single_core_proofs/scp_list_predicates.h"


/*@
// This predicate represents the memory corresponding to an
// initialised instance of type `TCB_t` aka `tskTaskControlBlock`.
// The predicate itself is not used during the verification of 
// `vTaskSwitchContext`. However, we keep it around to allow proof authors to
// validate that the predicates below indeed capture specific segments of a TCB.
predicate TCB_p(TCB_t * tcb, uint32_t ulFreeBytesOnStack) =
    malloc_block_tskTaskControlBlock(tcb) &*&
    tcb->pxStack |-> ?stackPtr &*&
    tcb->pxTopOfStack |-> ?topPtr &*&
    stack_p(stackPtr, ?ulStackDepth, topPtr, 
              ulFreeBytesOnStack, ?ulUsedCells, ?ulUnalignedBytes) &*&

    xLIST_ITEM(&tcb->xStateListItem, _, _, _, _, _) &*&
    struct_xLIST_ITEM_padding(&tcb->xStateListItem) &*&
    xLIST_ITEM(&tcb->xEventListItem, _, _, _, _, _) &*&
    struct_xLIST_ITEM_padding(&tcb->xEventListItem) &*&
    
    tcb->uxPriority |-> _ &*&

    tcb->xTaskRunState |-> ?gTaskRunState &*&
    tcb->xIsIdle |-> _ &*&
    
    // Assumes macro `configMAX_TASK_NAME_LEN` evaluates to 16.
    chars_(tcb->pcTaskName, 16, _) &*&

    tcb->uxCriticalNesting |-> ?uxCriticalNesting &*&
    tcb->uxTCBNumber |-> _ &*&
    tcb->uxTaskNumber |-> _ &*&
    tcb->uxBasePriority |-> _ &*&
    tcb->uxMutexesHeld |-> _ &*&

    // void * pvThreadLocalStoragePointers[ 5 ];
    pointers(tcb->pvThreadLocalStoragePointers, 5, _) &*&

    // We assume that the macro `configTASK_NOTIFICATION_ARRAY_ENTRIES`
    // evaluates to 1.
    integers_(tcb->ulNotifiedValue, 4, false, 1, _) &*&
    uchars((unsigned char*) tcb->ucNotifyState, 1, _) &*&

    tcb->ucDelayAborted |-> _;
@*/

/*@ 
// This predicate represents write access to a TCB's stack.
predicate TCB_stack_p(TCB_t* tcb, uint32_t ulFreeBytesOnStack) =
    tcb->pxStack |-> ?stackPtr &*&
    tcb->pxTopOfStack |-> ?topPtr &*&
    stack_p(stackPtr, ?ulStackDepth, topPtr, 
              ulFreeBytesOnStack, ?ulUsedCells, ?ulUnalignedBytes);

// This predicate represents write access to the run state of a TCB.
predicate TCB_runState_p(TCB_t* tcb, TaskRunning_t state;) =
    tcb->xTaskRunState |-> state;

// This predicate represents write access to the nesting level of a TCB.
// Entering a critical section increases the nesting level. Leaving it,
// decreases it.
predicate TCB_criticalNesting_p(TCB_t* tcb, UBaseType_t uxCriticalNesting) =
    tcb->uxCriticalNesting |-> uxCriticalNesting;
@*/

#endif /* TASKS_GH */