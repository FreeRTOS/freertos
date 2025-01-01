#ifndef TASK_RUNNING_STATES_H
#define TASK_RUNNING_STATES_H

/* The source file `tasks.c` defines macros to denote the running states of
 * tasks:
 * - `taskTASK_NOT_RUNNING` == -1
 * - `taskTASK_YIELDING` == -2
 * - state >= 0 => task is running on core with ID `state`
 * We cannot import theses definitions into our proof headers. Hence, we define
 * our own macros and proof in `tasks.c` that they match.
 */

#include "portmacro.h"  // defines `BaseType_t`

/* Indicates that the task is not actively running on any core. */
//VF_macro #define taskTASK_NOT_RUNNING    ( BaseType_t ) ( -1 )

/* Indicates that the task is actively running but scheduled to yield. */
//VF_macro #define taskTASK_YIELDING       ( BaseType_t ) ( -2 )


/* Verify that the preprocessor and our VeriFast proofs evaluate
 * `taskTASK_NOT_RUNNING` to the same values.
 */
void validate_taskTASK_NOT_RUNNING_value()
//@ requires true;
//@ ensures true;
{
    //@ TaskRunning_t gVal = taskTASK_NOT_RUNNING;
    TaskRunning_t val = taskTASK_NOT_RUNNING;
    //@ assert( gVal == val );
}

/* Verify that the preprocessor and our VeriFast proofs evaluate
 * `taskTASK_YIELDING` to the same values.
 */
void validate_taskTASK_YIELDING_value()
//@ requires true;
//@ ensures true;
{
    //@ TaskRunning_t gVal = taskTASK_YIELDING;
    TaskRunning_t val = taskTASK_YIELDING;
    //@ assert( gVal == val );
}

#endif /* TASK_RUNNING_STATES_H */