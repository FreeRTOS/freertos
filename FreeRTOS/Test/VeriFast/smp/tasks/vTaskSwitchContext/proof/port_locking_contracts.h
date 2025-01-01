#ifndef PORT_CONTRACTS_H
#define PORT_CONTRACTS_H

/* This file defines function contracts for the macros used to invoke
 * synchronization mechanisms, e.g., masking interrupts and acquiring locks.
 * The definitions of these macros are port-specific and involve inline
 * assembly. VeriFast cannot reason about assembly. Hence, we have to
 * abstract the assembly's semantics with these contracts.
 * 
 * Note that we cannot verify that the contracts' correctness. We have to treat
 * their correctness as a proof assumption.
 * 
 * Moreover, together with the invariants defined in the proof header
 * `lock_predicates.h`, the below contracts define the locking discipline that
 * our proof relies on. The file `lock_predicates.h` contains a more detailed
 * explanation of the locking discipline.
 * 
 * In short: 
 * - Data that is only meant to be accessed by the a specific core is protected
 *   by deactivating interrupts on this core. Access permissions are expressed
 *   by `coreLocalInterruptInv_p`.
 * - The task lock and the ISR lock (i.e. interrupt lock) themselves protect
 *   data and code regions irrelevant to the switch-context proof. Hence,
 *   the respective invariants are left abstract, cf. `taskLockInv_p` and
 *   `isrLockInv_p`.
 * - FreeRTOS' locking discipline demands that the task lock is acquired before
 *   and released after the ISR lock. The contracts defined below ensure that
 *   we follow this locking discipline.
 * - The ready lists and the task run states (i.e. the data most important to
 *   the context-switch proof) is protected by a combination of the task lock
 *   and the ISR lock. That is, this data must only be accessed when both
 *   locks have been acquired in the right order. The invariant 
 *   `taskISRLockInv_p` expresses these access rights. `lock_predicates.h`
 *   defines lemmas to produce and consume this invariant. The lemmas ensure
 *   that we only produce the invariant when both locks have been acquired in
 *   the right order.
 */

// We want our proofs to hold for an arbitrary number of cores.
#undef portGET_CORE_ID
#define portGET_CORE_ID() VF__get_core_num()

/* FreeRTOS core id is always zero based.*/
static uint VF__get_core_num(void);
//@ requires true;
/*@ ensures 0 <= result &*& result < configNUM_CORES &*&
            result == coreID_f();
@*/

/*@
// This contant allows proofs to talk about the ID of the core that the
// function we verify is running on. The verified function's contract must
// ensure that this constant holds the value of the current core.
fixpoint uint coreID_f();

lemma void coreID_f_range();
requires true;
ensures 0 <= coreID_f() &*& coreID_f() < configNUM_CORES;
@*/




/* In FreeRTOS interrupts are masked to protect core-local data.
 * The invariant `coreLocalInterruptInv_p` expresses what data the masking
 * of interrupts protects on a specific core, cf., `lock_predicates.h`.
 * 
 * Deactivating the interrupts on the current core produces the invariant
 * `coreLocalInterruptInv_p()` and thereby gives us the permission to access
 * the protected data.
 */
#undef portDISABLE_INTERRUPTS
#define portDISABLE_INTERRUPTS  VF__portDISABLE_INTERRUPTS
uint32_t VF__portDISABLE_INTERRUPTS();
//@ requires interruptState_p(?coreID, ?state);
/*@ ensures result == state &*& 
            interruptState_p(coreID, ?newState) &*&
            interruptsDisabled_f(newState) == true &*&
            interruptsDisabled_f(state) == true
               ? newState == state 
               : coreLocalInterruptInv_p();
@*/


/* This macro is used to restore the interrupt state (activated or deactivated)
 * to a specific value. When an invokation sets the state from deactivated to
 * activated, the invariant `coreLocalInterruptInv_p()` is consumed.
 * Thereby, we lose the permission to access the core-local data protected
 * by the deactivation of interrupts on this core.
 */
#undef portRESTORE_INTERRUPTS
#define portRESTORE_INTERRUPTS(ulState) VF__portRESTORE_INTERRUPTS(ulState)
void VF__portRESTORE_INTERRUPTS(uint32_t ulState);
/*@ requires interruptState_p(?coreID, ?tmpState) &*&
             (interruptsDisabled_f(tmpState) == true && interruptsDisabled_f(ulState) == false)
                ? coreLocalInterruptInv_p()
                : true;
 @*/
/*@ ensures interruptState_p(coreID, ulState);
@*/


/* This macro is used to acquire the task lock. The task lock on its own
 * protects data and core regions that are not relevant to the context-switch
 * proof. Hence, an invocation produces an abstract invariant `taskLockInv_p()`
 * and updates the locking history `locked_p(...)` to log that the task log
 * has been acquired.
 * 
 * FreeRTOS' locking discipline requires that the task lock must be acquired
 * before the ISR lock. The precondition `locked_p(nil)` only allows
 * invocations of this macro when no lock has been acquired, yet.
 */
#undef portGET_TASK_LOCK
#define portGET_TASK_LOCK  VF__portGET_TASK_LOCK
void VF__portGET_TASK_LOCK();
//@ requires [?f]taskLock_p() &*& locked_p(nil);
//@ ensures taskLockInv_p() &*& locked_p( cons( pair(f, taskLockID_f()), nil) );


/* This macro is used to release the task lock. An invocation consumes the 
 * task lock invariant `taskLockInv_p` and updates the locking history
 * `locked_p(...)` to reflect the release.
 * 
 * FreeRTOS' locking discipline demands that the task lock must be acquired
 * before and released after the ISR lock. The precondition
 * `locked_p( cons( pair(?f, taskLockID_f()), nil) )` only allows calls to this
 * macro when we can prove that we only hold the task lock.
 * */
#undef portRELEASE_TASK_LOCK
#define portRELEASE_TASK_LOCK VF__portRELEASE_TASK_LOCK
void VF__portRELEASE_TASK_LOCK();
//@ requires taskLockInv_p() &*& locked_p( cons( pair(?f, taskLockID_f()), nil) );
//@ ensures [f]taskLock_p() &*& locked_p(nil);


/* This macro is used to acquire the ISR lock (i.e. interrupt lock). An
 * invocation produces the abstract ISR lock invariant `isrLock_p` and
 * updates the locking history `locked_p(...)` to reflect that the lock has
 * been acquired.
 */
#undef portGET_ISR_LOCK
#define portGET_ISR_LOCK  VF__portGET_ISR_LOCK
void VF__portGET_ISR_LOCK();
//@ requires [?f]isrLock_p() &*& locked_p(?heldLocks);
//@ ensures isrLockInv_p() &*& locked_p( cons( pair(f, isrLockID_f()), heldLocks) );


/* This macro is used to release the ISR lock (i.e. interrupt lock). A call
 * consumes the ISR lock invariant and updates the locking history 
 * `locked_p(...)` to reflect the release.
 */
#undef portRELEASE_ISR_LOCK
#define portRELEASE_ISR_LOCK VF__portRELEASE_ISR_LOCK
void VF__portRELEASE_ISR_LOCK();
//@ requires isrLockInv_p() &*& locked_p( cons( pair(?f, isrLockID_f()), ?heldLocks) );
//@ ensures [f]isrLock_p() &*& locked_p(heldLocks);


#endif /* PORT_CONTRACTS_H */