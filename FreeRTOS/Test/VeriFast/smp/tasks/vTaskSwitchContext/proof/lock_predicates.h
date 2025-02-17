#ifndef LOCK_PREDICATES_H
#define LOCK_PREDICATES_H

#include "task_running_states.h"


#include "verifast_lists_extended.h"


/* ----------------------------------------------------------------------
 * Locking discipline explained:
 * FreeRTOS uses the following synchronization mechanisms:
 * - Deactivating interrupts:
 *   Some data is only meant to be accessed on a specific core C. Such data
 *   may only be accessed after interrupts on core C have been deactivated. 
 *   For instance the global array `pxCurrentTCBs` in `tasks.c` has an entry for
 *   every core. `pxCurrentTCBs[C]` stores a pointer to the TCB of the task
 *   running on core C. Core C is always allowed to read `pxCurrentTCBs[C]`.
 *   However, writing requires the interrupts on core C to be deactivated.
 *
 *   The resources protected by disabling interrupts are represented by the
 *   predicate `coreLocalInterruptInv_p` defined below.
 *
 * - task lock:
 *   The task lock is used to protect ciritical sections and resources from 
 *   being accessed by multiple tasks simultaneously. The resources protected
 *   by the task lock are represented by the abstract predicate `taskLockInv_p`
 *   defined below. This proof does not deal with resources or code segments
 *   only protected by the task lock. Hence, we leave the predicate abstract.
 *   
 * - ISR lock:
 *   The ISR/ interrupt lock is used to protect critical sections and resources
 *   from being accessed by multiple interrupts simultaneously. The resources 
 *   protected by the ISR lock are represented by the abstract predicate
 *   `isrLockInv_p` defined below. This proof does not deal with resources or 
 *   code segments only protected by the ISR lock. Hence, we leave the predicate
 *   abstract.
 * 
 * - task lock + ISR lock:
 *   Access to certain resources and ciritical sections are protected by both
 *   the task lock and the ISR lock. For these, it is crucial that we first
 *   acquire the task lock and then the ISR lock. Likewise, we must release them
 *   in opposite order. Failure to comply with this order may lead to deadlocks. 
 *   The resources protected by both locks are the main resources this proof
 *   deals with. These include the ready lists and the certain access rights
 *   to the tasks' run states. The access rights protected by both locks are
 *   represented by the predicate `taskISRLockInv_p` defined below.
 *   Once both locks have been acquired in the right order, this lock invariant 
 *   can be produced by calling the lemma `produce_taskISRLockInv`. Before the 
 *   locks can be released, the invariant must be consumed by calling 
 *   `consume_taskISRLockInv`. Both lemmas are defined below.
*/


/* ----------------------------------------------------------------------
 * Core local data and access restrictions.
 * Some data in FreeRTOS such as the pointer to TCB of the task running
 * on core `C` may only be accessed from core `C`. Such core-local data is
 * protected by deactivating interrupts.
 */

/*@ 
// Represents the state of interrupts (i.e. activated or deactivated) on a
// specific core. The state corresponds to the value of the special register
// used for interrupt masking.
predicate interruptState_p(uint32_t coreID, uint32_t state);

// Given an interrupt state (i.e. the value of the special register used to
// control interrupt masking), this function returns whether the state expresses
// that interrupts are deactivated.
fixpoint bool interruptsDisabled_f(uint32_t);


// This predicate expresses that the core we are currently reasoning about
// (expressed by constant `coreID_f`) is allowed to access the core-local data
// protected by interrupt masking.
predicate coreLocalInterruptInv_p() =
    // Read permission to the entry of `pxCurrentTCBs` that stores a pointer
    // to the task currenlty running on this core
    [0.5]pointer(&pxCurrentTCBs[coreID_f], ?currentTCB) 
    &*&
    // Write permission to the entry of `xYieldPendings` for the current core
    integer_(&xYieldPendings[coreID_f], sizeof(BaseType_t), true, _) 
    &*&
    // Write permission to the "critical nesting" field of the task
    // currently scheduled on this core. The field allows us to check whether
    // the task is currently in a critical section. Necessary to check whether,
    // we are allowed to context switch.
    TCB_criticalNesting_p(currentTCB, ?gCriticalNesting);
@*/


/* ----------------------------------------------------------------------
 * Predicates relevant for all locks
 */

/*@
// This predicate is used to remember which locks we're currently holding. Each
// Each constists of a pair `(f,id)`. `f` is the fraction of the lock we held
// before acquiring. Remembering the fraction is important to ensure that we 
// reproduce the right fraction of the lock predicate when we release the lock.
// Otherwise, we can run into inconsistencies.
// `id` is the ID of the acquired lock, i.e., either `taskLockID_f` or 
// `isrLockID_f`.
predicate locked_p(list< pair<real, int> > lockHistory);
@*/



/* ----------------------------------------------------------------------
 * Task lock
 */

/*@
fixpoint int taskLockID_f();

// Represents an unacquired task lock.
predicate taskLock_p(); 

// Represents the invariant associated with the the task lock, i.e.,
// access permissions to the resources and code regions protected by the lock.
// These are not relevant to the context-switch proof. Therefore, we leave the
// predicate abstract.
predicate taskLockInv_p();
@*/

/* ----------------------------------------------------------------------
 * ISR lock
 */

/*@
fixpoint int isrLockID_f();

// Represents an unacquired ISR lock.
predicate isrLock_p(); 

// Represents the invariant associated with the the ISR lock, i.e.,
// access permissions to the resources and code regions protected by the lock.
// These are not relevant to the context-switch proof. Therefore, we leave the
// predicate abstract.
predicate isrLockInv_p();
@*/


/* ----------------------------------------------------------------------
 * Resources protected by both locks.
 * Note that the task lock may never be acquired after the ISR lock.
 */

/*@
fixpoint int taskISRLockID_f();

// Represents the access rights protected by both the task and the ISR lock.
// Note that FreeRTOS' locking discipline demands the the task lock must be
// acquired before the ISR lock. Once, both locks have been acquired in the
// right order, ths invariant can be produced by invoking the lemma 
// `produce_taskISRLockInv` and it can be consumed by invoking 
// `consume_taskISRLockInv`. The lemmas ensure that we follow the locking
// discipline.
//
// This invariant expresses fine grained access rights to the following:
// - some global variables:
//   + Read permission to the entry of `pxCurrentTCBs` that stores a pointer
//     to the task currenly running on the core `coreID_f` our proof currently
//     considers. Together with the read permission from
//     `coreLocalInterruptInv_p` we get write access to this entry once
//     interrupts have been deactivated and both locks have been acquired.
//   + Write permission to `uxSchedulerSuspended`.
//   + Write permission to `xSchedulerRunning`.
//   + Write permission to `uxTopReadyPriority`. This variable stores to top
//     priority for which there is a task that is ready to be scheduled.
// - Write access to the ready lists.
// - Fine-grained access permissions for task run states:
//   + (RP-All) Read permission for every task.
//   + (RP-Current) Read permission for task currently scheduled on this core.
//     Together, (RP-All) and (RP-Current) give us a write permission for 
//     task scheduled on this core.
//   + (RP-Unsched) Read permissions for unscheduled tasks.
//     Together, (RP-All) and (RP-Unsched) give us write permissions for all
//     unscheduled tasks.
//   Note that these permissions do not allow us to change the run state of any
//   task that is currently scheduled on another core.
predicate taskISRLockInv_p() = 
    _taskISRLockInv_p(_);


// Auxiliary predicate. Equal to the lock invariant above but exposes
// some details.
predicate _taskISRLockInv_p(UBaseType_t gTopReadyPriority) = 
    // Access to global variables
        [0.5]pointer(&pxCurrentTCBs[coreID_f], ?gCurrentTCB) &*&
        integer_((void*) &uxSchedulerSuspended, sizeof(UBaseType_t), false, _) &*&
        integer_(&xSchedulerRunning, sizeof(BaseType_t), true, _)
    &*&
    // top ready priority must be in range
        integer_((void*) &uxTopReadyPriority, sizeof(UBaseType_t), false, gTopReadyPriority) &*&
        0 <= gTopReadyPriority &*& gTopReadyPriority < configMAX_PRIORITIES
    &*&
    // tasks / TCBs
        exists_in_taskISRLockInv_p(?gTasks, ?gStates)
        &*&
        // (RP-All) Read permissions for every task
        //          and recording of task states in state list
        // (∀t ∈ gTasks. 
        //      [1/2]TCB_runState_p(t, _))
        // ∧ 
        // ∀i. ∀t. gTasks[i] == t -> gStates[i] == t->xTaskRunState    
            foreach(gTasks, readOnly_TCB_runState_p(gTasks, gStates))
        &*&
        // (RP-Current) Read permission for task currently scheduled on this core
        // (RP-All) + (RP-Current) => Write permission for scheduled task
            [1/2]TCB_runState_p(gCurrentTCB, ?gCurrentTCB_state) &*&
            (gCurrentTCB_state == coreID_f() || gCurrentTCB_state == taskTASK_YIELDING) &*&
            nth(index_of(gCurrentTCB, gTasks), gStates) == gCurrentTCB_state
        &*&
        // (RP-Unsched) Read permissions for unscheduled tasks
        // (RP-All) + (RP-Unsched) => Write permissions for unscheduled tasks
        // ∀t ∈ tasks. t->xTaskState == taskTASK_NOT_RUNNING
        //             -> [1/2]shared_TCB_p(t, taskTASK_NOT_RUNNING)
            foreach(gTasks, readOnly_TCB_runState_IF_not_running_p(gTasks, gStates))
        &*&
        readyLists_p(?gCellLists, ?gOwnerLists) 
        &*&
        // gTasks contains all relevant tasks
            mem(gCurrentTCB, gTasks) == true
            &*&
            // ∀l ∈ gOwnerLists. l ⊆ gTasks
                forall(gOwnerLists, (superset)(gTasks)) == true;


lemma void produce_taskISRLockInv();
requires locked_p(?heldLocks) &*&
         heldLocks == cons(?i, cons(?t, nil)) &*&
         i == pair(?f_isr, isrLockID_f()) &*&
         t == pair(?f_task, taskLockID_f());
ensures locked_p( cons( pair(_, taskISRLockID_f()), heldLocks) ) &*&
        taskISRLockInv_p();


lemma void consume_taskISRLockInv();
requires locked_p( cons( pair(_, taskISRLockID_f()), ?otherLocks) ) &*&
         taskISRLockInv_p();
ensures  locked_p(otherLocks);



// Auxiliary predicate to assing names to existentially quantified variables.
// Having multiple `exists` chunks on the heap makes matching against their
// arguments ambiguous in most cases.
predicate exists_in_taskISRLockInv_p(list<void*> gTasks,
                                     list<TaskRunning_t> gStates) =
    exists(gTasks) &*&
    exists(gStates) &*&
    length(gTasks) == length(gStates) &*&
    distinct(gTasks) == true;

// Auxiliary function that allows us to partially apply the list argument.
//
// Notes:
// - Partial application of fixpoint functions in VeriFast is not documented.
//   The syntax for partially application is `(<fixpoint_fct>)(<first_arg>)`
// - VeriFast only supports partially applying the first argument, e.g.,
//   `(mem)(0)` is allowed but `(mem)(_)(nil)` is not.
fixpoint bool mem_list_elem<t>(list<t> xs, t x) {
    return mem(x, xs);
}

// Auxiliary predicate to allow foreach-quantification about fraction
// and reflection of `t->xTaskRunState` in state list.
predicate_ctor readOnly_TCB_runState_p
            (list<void*> tasks, list<TaskRunning_t> states)
            (TCB_t* t;) =
    mem(t, tasks) == true &*&
    [1/2]TCB_runState_p(t, nth(index_of(t, tasks), states));

predicate_ctor readOnly_TCB_runState_IF_not_running_p
            (list<void*> tasks, list<TaskRunning_t> states)
            (TCB_t* t;) =
    mem(t, tasks) == true &*&
    nth(index_of(t, tasks), states) == taskTASK_NOT_RUNNING
        ? [1/2]TCB_runState_p(t, taskTASK_NOT_RUNNING)
        : true;
@*/





// -----------------------------------------------------------------------
// The following lemmas are necessary to prove that state updates preserve
// the lock invariant.

/*@
lemma void update_readOnly_TCB_runState(TCB_t* t, 
                                         list<void*> tasks,
                                         list<TaskRunning_t> states, 
                                         int updatedIndex,
                                         TaskRunning_t s)
requires readOnly_TCB_runState_p(tasks, states)(t) &*&
         updatedIndex != index_of(t, tasks) &*&
         mem(t, tasks) == true &*&
         length(tasks) == length(states);
ensures  readOnly_TCB_runState_p(tasks, update(updatedIndex, s, states))(t);
{
    list<TaskRunning_t> states2 = update(updatedIndex, s, states);
    int t_index = index_of(t, tasks);

    if( updatedIndex < 0 || updatedIndex >= length(states) ) {
        update_out_of_bounds(updatedIndex, s, states);
    } else {
        open readOnly_TCB_runState_p(tasks, states)(t);
        open [1/2]TCB_runState_p(t, nth(t_index, states));

        mem_index_of(t, tasks);
        nth_update(t_index, updatedIndex, s, states);
        assert( nth(t_index, states) == nth(t_index, states2) );

        close [1/2]TCB_runState_p(t, nth(t_index, states2));
        close readOnly_TCB_runState_p(tasks, states2)(t);
    }
}


lemma void update_foreach_readOnly_TCB_runState(TCB_t* updatedTask, 
                                                 list<void*> tasks,
                                                 list<void*> subTasks,
                                                 list<TaskRunning_t> states, 
                                                 list<TaskRunning_t> states2, 
                                                 TaskRunning_t s)
requires
    mem(updatedTask, tasks) == true &*&
    length(tasks) == length(states) &*&
    foreach(subTasks, readOnly_TCB_runState_p(tasks, states)) &*&
    states2 == update(index_of(updatedTask, tasks), s, states) &*&
    distinct(tasks) == true &*&
    mem(updatedTask, subTasks) == false &*&
    subset(subTasks, tasks) == true;
ensures
    foreach(subTasks, readOnly_TCB_runState_p(tasks, states2));
{
    switch(subTasks) {
        case nil:   
            open foreach(nil, readOnly_TCB_runState_p(tasks, states));
            close foreach(nil, readOnly_TCB_runState_p(tasks, states2));
        case cons(h, rest):
            int index = index_of(updatedTask, tasks);
            open foreach(subTasks, readOnly_TCB_runState_p(tasks, states));
            assert( updatedTask != h );
            index_of_different(updatedTask, h, tasks);
            assert( index != index_of(h, tasks) );
            update_readOnly_TCB_runState(h, tasks, states, index, s);
            assert( mem(updatedTask, rest) == false );
            update_foreach_readOnly_TCB_runState(updatedTask, tasks, rest, 
                                                  states, states2, s);
            close foreach(subTasks, readOnly_TCB_runState_p(tasks, states2));
    }
}


lemma void close_updated_foreach_readOnly_TCB_runState(TCB_t* updatedTask, 
                                                        list<void*> tasks,
                                                        list<TaskRunning_t> states, 
                                                        list<TaskRunning_t> states2, 
                                                        TaskRunning_t s)
requires
    mem(updatedTask, tasks) == true &*&
    length(states) == length(tasks) &*&
    distinct(tasks) == true &*&
    foreach(remove(updatedTask, tasks), readOnly_TCB_runState_p(tasks, states)) &*&
    states2 == update(index_of(updatedTask, tasks), s, states) &*&
    [1/2]TCB_runState_p(updatedTask, s);
ensures
    foreach(tasks, readOnly_TCB_runState_p(tasks, states2));
{
    distinct_mem_remove(updatedTask, tasks);
    remove_result_subset(updatedTask, tasks);

    close readOnly_TCB_runState_p(tasks, states2)(updatedTask);
    update_foreach_readOnly_TCB_runState(updatedTask, tasks, 
                                          remove(updatedTask, tasks),
                                          states, states2, s);
    foreach_unremove(updatedTask, tasks);
}


lemma void stopUpdate_foreach_readOnly_TCB_runState_IF_not_running
            (TCB_t* stoppedTask, list<void*> tasks, list<void*> subTasks,
             list<TaskRunning_t> states, list<TaskRunning_t> states2)
requires 
    distinct(tasks) == true &*&
    distinct(subTasks) == true &*&
    length(tasks) == length(states) &*&
    foreach(subTasks, readOnly_TCB_runState_IF_not_running_p(tasks, states)) &*&
    states2 == update(index_of(stoppedTask, tasks), taskTASK_NOT_RUNNING, states) &*&
    nth(index_of(stoppedTask, tasks), states) != taskTASK_NOT_RUNNING &*&
    subset(subTasks, tasks) == true &*&
    mem(stoppedTask, tasks) == true &*&
    mem(stoppedTask, subTasks)
        ? [1/2]TCB_runState_p(stoppedTask, taskTASK_NOT_RUNNING)
        : true;
ensures 
    foreach(subTasks, readOnly_TCB_runState_IF_not_running_p(tasks, states2));
{
    switch(subTasks) {
        case nil:
            open foreach(nil, readOnly_TCB_runState_IF_not_running_p(tasks, states));
            close foreach(nil, readOnly_TCB_runState_IF_not_running_p(tasks, states2));
        case cons(h, t):
            if( h == stoppedTask ) {
                assert( remove(stoppedTask, subTasks) == t );
                distinct_mem_remove(stoppedTask, subTasks);
                assert( mem(stoppedTask, t) == false );
                mem_index_of(stoppedTask, tasks);
            } else {
                mem_index_of(stoppedTask, tasks);
                nth_update(index_of(h, tasks), index_of(stoppedTask, tasks), taskTASK_NOT_RUNNING, states);
                index_of_different(h, stoppedTask, tasks);
                assert( index_of(h, tasks) != index_of(stoppedTask, tasks) );
                assert( nth(index_of(h, tasks), states) == nth(index_of(h, tasks), states2) );
            }

            nth_update(index_of(stoppedTask, tasks), index_of(stoppedTask, tasks), taskTASK_NOT_RUNNING, states);
            open foreach(subTasks, readOnly_TCB_runState_IF_not_running_p(tasks, states));
            open readOnly_TCB_runState_IF_not_running_p(tasks, states)(h);
            assert( nth(index_of(stoppedTask, tasks), states2) ==  taskTASK_NOT_RUNNING );
            close readOnly_TCB_runState_IF_not_running_p(tasks, states2)(h);
            stopUpdate_foreach_readOnly_TCB_runState_IF_not_running
                (stoppedTask, tasks, t, states, states2);
            close foreach(subTasks, readOnly_TCB_runState_IF_not_running_p(tasks, states2));
    }
}

lemma void updateUnaffectedStates_in_foreach_readOnly_TCB_runState_IF_not_running
            (TCB_t* updatedTask, list<void*> tasks, list<void*> subTasks,
             list<TaskRunning_t> states, list<TaskRunning_t> updatedStates,
             TaskRunning_t s)
requires 
    distinct(tasks) == true &*&
    distinct(subTasks) == true &*&
    length(tasks) == length(states) &*&
    mem(updatedTask, tasks) == true &*&
    mem(updatedTask, subTasks) == false &*&
    subset(subTasks, tasks) == true &*&
    foreach(subTasks, readOnly_TCB_runState_IF_not_running_p(tasks, states)) &*&
    updatedStates == update(index_of(updatedTask, tasks), s, states);
ensures 
    foreach(subTasks, readOnly_TCB_runState_IF_not_running_p(tasks, updatedStates));
{
    switch(subTasks) {
        case nil:
            open  foreach(nil, readOnly_TCB_runState_IF_not_running_p(tasks, states));
            close foreach(nil, readOnly_TCB_runState_IF_not_running_p(tasks, updatedStates));
        case cons(h, t):
            open foreach(subTasks, readOnly_TCB_runState_IF_not_running_p(tasks, states));

            // Prove that update preserves state of `h`.
            index_of_different(h, updatedTask, tasks);
            nth_update(index_of(h, tasks), index_of(updatedTask, tasks), s, states);
            assert( nth(index_of(h, tasks), states) == nth(index_of(h, tasks), updatedStates) );

            open readOnly_TCB_runState_IF_not_running_p(tasks, states)(h);
            close readOnly_TCB_runState_IF_not_running_p(tasks, updatedStates)(h);
            updateUnaffectedStates_in_foreach_readOnly_TCB_runState_IF_not_running
                (updatedTask, tasks, t, states, updatedStates, s);
            close foreach(subTasks, readOnly_TCB_runState_IF_not_running_p(tasks, updatedStates));
    }
}

lemma void startUpdate_foreach_readOnly_TCB_runState_IF_not_running
            (TCB_t* startedTask, list<void*> tasks,
             list<TaskRunning_t> states, list<TaskRunning_t> updatedStates,
             int coreID)
requires 
    distinct(tasks) == true &*&
    length(tasks) == length(states) &*&
    mem(startedTask, tasks) == true &*&
    foreach(remove(startedTask, tasks), readOnly_TCB_runState_IF_not_running_p(tasks, states)) &*&
    updatedStates == update(index_of(startedTask, tasks), coreID, states) &*&
    0 <= coreID &*& coreID < configNUM_CORES;
ensures 
    foreach(tasks, readOnly_TCB_runState_IF_not_running_p(tasks, updatedStates));
{
    distinct_remove(startedTask, tasks);
    distinct_mem_remove(startedTask, tasks);
    remove_result_subset(startedTask, tasks);
    updateUnaffectedStates_in_foreach_readOnly_TCB_runState_IF_not_running
        (startedTask, tasks, remove(startedTask, tasks), states, updatedStates, 
         coreID);
                             
    assert( foreach(remove(startedTask, tasks), readOnly_TCB_runState_IF_not_running_p(tasks, updatedStates)) );
    close readOnly_TCB_runState_IF_not_running_p(tasks, updatedStates)(startedTask);
    foreach_unremove(startedTask, tasks);
}

lemma void scheduleRunning_in_foreach_readOnly_TCB_runState_IF_not_running
                (TCB_t* runningTask, list<void*> tasks,
                list<TaskRunning_t> states, list<TaskRunning_t> updatedStates,
                int coreID)
requires 
    distinct(tasks) == true &*&
    length(tasks) == length(states) &*&
    mem(runningTask, tasks) == true &*&
    (nth(index_of(runningTask, tasks), states) == coreID
        || nth(index_of(runningTask, tasks), states) == taskTASK_YIELDING)
    &*&
    foreach(tasks, readOnly_TCB_runState_IF_not_running_p(tasks, states)) &*&
    updatedStates == update(index_of(runningTask, tasks), coreID, states) &*&
    0 <= coreID &*& coreID < configNUM_CORES;
ensures 
    foreach(tasks, readOnly_TCB_runState_IF_not_running_p(tasks, updatedStates)) &*&
    nth(index_of(runningTask, tasks), updatedStates) == coreID;
{
    switch(tasks) {
        case nil:
            open  foreach(nil, readOnly_TCB_runState_IF_not_running_p(tasks, states));
            close foreach(nil, readOnly_TCB_runState_IF_not_running_p(tasks, updatedStates));
        case cons(h, t):
            foreach_remove(runningTask, tasks);

            distinct_remove(runningTask, tasks);
            distinct_mem_remove(runningTask, tasks);
            remove_result_subset(runningTask, tasks);
            updateUnaffectedStates_in_foreach_readOnly_TCB_runState_IF_not_running
                (runningTask, tasks, remove(runningTask, tasks), 
                 states, updatedStates, coreID);
            
            open readOnly_TCB_runState_IF_not_running_p(tasks, states)(runningTask);
            close readOnly_TCB_runState_IF_not_running_p(tasks, updatedStates)(runningTask);

            foreach_unremove(runningTask, tasks);
    }
}
@*/


/*@
lemma list<TaskRunning_t> def_state1(list<void*> tasks,
                                     list<TaskRunning_t> states,
                                     TCB_t* currentTask,
                                     TCB_t* readyTask)
requires 
    distinct(tasks) == true &*&
    length(tasks) == length(states) &*&
    currentTask != readyTask &*&
    mem(currentTask, tasks) == true &*&
    mem(readyTask, tasks) == true &*&
    nth(index_of(readyTask, tasks), states) == taskTASK_NOT_RUNNING;
ensures
    result == update(index_of(currentTask, tasks), taskTASK_NOT_RUNNING, states) &*&
    nth(index_of(readyTask, tasks), result) == taskTASK_NOT_RUNNING &*&
    nth(index_of(currentTask, tasks), result) == taskTASK_NOT_RUNNING;
{
    list<TaskRunning_t> states1 =  
        update(index_of(currentTask, tasks), taskTASK_NOT_RUNNING, states);
    
    mem_index_of(currentTask, tasks);
    mem_index_of(readyTask, tasks);
    nth_update(index_of(readyTask, tasks), index_of(currentTask, tasks), taskTASK_NOT_RUNNING, states);

    return states1;
}

lemma list<TaskRunning_t> def_state2(list<void*> tasks,
                                     list<TaskRunning_t> states,
                                     TCB_t* currentTask,
                                     TCB_t* readyTask,
                                     int coreID)
requires 
    distinct(tasks) == true &*&
    length(tasks) == length(states) &*&
    currentTask != readyTask &*&
    mem(currentTask, tasks) == true &*&
    mem(readyTask, tasks) == true &*&
    nth(index_of(readyTask, tasks), states) == taskTASK_NOT_RUNNING &*&
    nth(index_of(currentTask, tasks), states) != taskTASK_NOT_RUNNING &*&
    0 <= coreID &*& coreID < configNUM_CORES;
ensures
    result == 
        update(index_of(readyTask, tasks), coreID,
            update(index_of(currentTask, tasks), taskTASK_NOT_RUNNING, states)) 
    &*&
    nth(index_of(readyTask, tasks), result) == coreID &*&
    nth(index_of(currentTask, tasks), result) == taskTASK_NOT_RUNNING;
{
    list<TaskRunning_t> states1 = def_state1(tasks, states, currentTask, readyTask);

    list<TaskRunning_t> states2 = 
        update(index_of(readyTask, tasks), coreID, states1);
    
    return states2;
}
@*/






#endif /* LOCK_PREDICATES_H */