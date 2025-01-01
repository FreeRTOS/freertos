/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* The MPU version of port.c includes and excludes functions depending on the
settings within this file.  Therefore, to ensure all the functions in port.c
build, this configuration file has all options turned on. */
#define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY 1
#define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS 0
#define configUSE_PREEMPTION                    1
#define configTICK_RATE_HZ                        ( 1000 )
#define configUSE_PORT_OPTIMISED_TASK_SELECTION    1
#define configUSE_QUEUE_SETS                    1
#define configUSE_IDLE_HOOK                        1
#define configUSE_TICK_HOOK                        1
#define configCPU_CLOCK_HZ                        48000000
#define configMAX_PRIORITIES                    ( 5 )
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 120 )
#define configTOTAL_HEAP_SIZE                    ( ( size_t ) ( 15 * 1024 ) )
#define configMAX_TASK_NAME_LEN                    ( 10 )
#define configUSE_TRACE_FACILITY                1
#define configUSE_16_BIT_TICKS                    0
#define configIDLE_SHOULD_YIELD                    1
#define configUSE_MUTEXES                        1
#define configQUEUE_REGISTRY_SIZE                5
#define configCHECK_FOR_STACK_OVERFLOW            2
#define configUSE_RECURSIVE_MUTEXES                1
#define configUSE_MALLOC_FAILED_HOOK            1
#define configUSE_APPLICATION_TASK_TAG            1
#define configUSE_COUNTING_SEMAPHORES            1
#define configSUPPORT_STATIC_ALLOCATION            0
#define configUSE_TICKLESS_IDLE                    0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS    2

/* Run time stats gathering definitions. */
#define configGENERATE_RUN_TIME_STATS            1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE()        0

/* This demo makes use of one or more example stats formatting functions.  These
format the raw data provided by the uxTaskGetSystemState() function in to human
readable ASCII form.  See the notes in the implementation of vTaskList() within
FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES             0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY        ( 2 )
#define configTIMER_QUEUE_LENGTH        5
#define configTIMER_TASK_STACK_DEPTH    ( configMINIMAL_STACK_SIZE )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet            1
#define INCLUDE_vTaskDelete                    1
#define INCLUDE_vTaskCleanUpResources        1
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil                1
#define INCLUDE_vTaskDelay                    1
#define INCLUDE_eTaskGetState                1
#define INCLUDE_xTimerPendFunctionCall        1
#define INCLUDE_xSemaphoreGetMutexHolder    1
#define INCLUDE_xTaskGetHandle                1
#define INCLUDE_xTaskGetCurrentTaskHandle    1
#define INCLUDE_xTaskGetIdleTaskHandle        1
#define INCLUDE_xTaskAbortDelay                1
#define INCLUDE_xTaskGetSchedulerState        1
#define INCLUDE_xTaskGetIdleTaskHandle        1
#define INCLUDE_uxTaskGetStackHighWaterMark 1
#define INCLUDE_uxTaskGetStackHighWaterMark2 1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
    /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
    #define configPRIO_BITS               __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS               8    /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY            0xf

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY     ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT( x ) if( ( x ) == 0 ) { portDISABLE_INTERRUPTS(); for( ;; ); }

/* LED not used at present, so just increment a variable to keep a count of the
number of times the LED would otherwise have been toggled. */
#define configTOGGLE_LED()    ulLED++

/* Definitions for the messages that can be sent to the check task. */
#define configREG_TEST_1_STILL_EXECUTING    ( 0 )
#define configREG_TEST_2_STILL_EXECUTING    ( 1 )
#define configTIMER_STILL_EXECUTING            ( 2 )
#define configPRINT_SYSTEM_STATUS            ( 3 )

/* Parameters that are passed into the third and fourth register check tasks
solely for the purpose of ensuring parameters are passed into tasks correctly. */
#define configREG_TEST_TASK_1_PARAMETER    ( ( void * ) 0x11112222 )
#define configREG_TEST_TASK_3_PARAMETER    ( ( void * ) 0x12345678 )
#define configREG_TEST_TASK_4_PARAMETER    ( ( void * ) 0x87654321 )

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_CONFIG_H */

