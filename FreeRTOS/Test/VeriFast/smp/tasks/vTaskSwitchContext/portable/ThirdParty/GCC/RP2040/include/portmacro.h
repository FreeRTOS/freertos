/*
 * FreeRTOS SMP Kernel V202110.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: MIT AND BSD-3-Clause
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

#ifndef PORTMACRO_H
    #define PORTMACRO_H

    #ifdef __cplusplus
        extern "C" {
    #endif

    #include "pico.h"
    #include "hardware/sync.h"

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
    #define portCHAR          char
    #define portFLOAT         float
    #define portDOUBLE        double
    #define portLONG          long
    #define portSHORT         short
    #define portSTACK_TYPE    uint32_t
    #define portBASE_TYPE     long

    typedef portSTACK_TYPE    StackType_t;
    typedef int32_t           BaseType_t;
    typedef uint32_t          UBaseType_t;

    #if ( configUSE_16_BIT_TICKS == 1 )
        typedef uint16_t     TickType_t;
        #define portMAX_DELAY              ( TickType_t ) 0xffff
    #else
        typedef uint32_t     TickType_t;
        #define portMAX_DELAY              ( TickType_t ) 0xffffffffUL

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
 * not need to be guarded with a critical section. */
        #define portTICK_TYPE_IS_ATOMIC    1
    #endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
    #define portSTACK_GROWTH      ( -1 )
    #define portTICK_PERIOD_MS    ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
    #define portBYTE_ALIGNMENT    8
    #ifdef VERIFAST
        /* Reason for rewrite: VeriFast does not support the attriibute `used`. 
         */
        #define portDONT_DISCARD    
    #else
        #define portDONT_DISCARD      __attribute__( ( used ) )
    #endif
    /* We have to use PICO_DIVIDER_DISABLE_INTERRUPTS as the source of truth rathern than our config,
     * as our FreeRTOSConfig.h header cannot be included by ASM code - which is what this affects in the SDK */
    #define portUSE_DIVIDER_SAVE_RESTORE !PICO_DIVIDER_DISABLE_INTERRUPTS
    #if portUSE_DIVIDER_SAVE_RESTORE
        #define portSTACK_LIMIT_PADDING 4
    #endif

/*-----------------------------------------------------------*/


/* Scheduler utilities. */
    extern void vPortYield( void );
    #define portNVIC_INT_CTRL_REG     ( *( ( volatile uint32_t * ) 0xe000ed04 ) )
    #define portNVIC_PENDSVSET_BIT    ( 1UL << 28UL )
    #define portYIELD()                                 vPortYield()
    #define portEND_SWITCHING_ISR( xSwitchRequired )    if( xSwitchRequired ) portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT
    #define portYIELD_FROM_ISR( x )                     portEND_SWITCHING_ISR( x )

/*-----------------------------------------------------------*/

/* Exception handlers */
    #if (configUSE_DYNAMIC_EXCEPTION_HANDLERS == 0)
        /* We only need to override the SDK's weak functions if we want to replace them at compile time */
        #define vPortSVCHandler isr_svcall
        #define xPortPendSVHandler isr_pendsv
        #define xPortSysTickHandler isr_systick
    #endif

/*-----------------------------------------------------------*/

/* Multi-core */
    #define portSUPPORT_SMP 1 /* this is an SMP build which means configNUM_CORES is relevant */
    #define portMAX_CORE_COUNT            2
    #ifndef configNUM_CORES
    #define configNUM_CORES               2
    #endif

    /* Requires for SMP */
    #define portCRITICAL_NESTING_IN_TCB   1

    /*-----------------------------------------------------------*/


    /* Check validity of number of cores specified in config */
    #if ( configNUM_CORES < 1 || portMAX_CORE_COUNT < configNUM_CORES )
    #error "Invalid number of cores specified in config!"
    #endif

    #if ( configTICK_CORE < 0 || configTICK_CORE > configNUM_CORES )
    #error "Invalid tick core specified in config!"
    #endif

    /* FreeRTOS core id is always zero based, so always 0 if we're running on only one core */
    #if configNUM_CORES == portMAX_CORE_COUNT
    #define portGET_CORE_ID() get_core_num()
    #else
    #define portGET_CORE_ID() 0
    #endif

    #define portCHECK_IF_IN_ISR() ({ \
        uint32_t ulIPSR;                                                  \
       __asm volatile ("mrs %0, IPSR" : "=r" (ulIPSR)::);             \
       ((uint8_t)ulIPSR)>0;})

    void vYieldCore(int xCoreID);
    #define portYIELD_CORE(a) vYieldCore(a)
    #define portRESTORE_INTERRUPTS(ulState) __asm volatile ("msr PRIMASK,%0"::"r" (ulState) : )

/*-----------------------------------------------------------*/

/* Critical section management. */

    #define portSET_INTERRUPT_MASK_FROM_ISR()         ({ uint32_t ulStateISR = portDISABLE_INTERRUPTS(); vTaskEnterCritical(); ulStateISR; })
    #define portCLEAR_INTERRUPT_MASK_FROM_ISR( x )    do { vTaskExitCritical(); portRESTORE_INTERRUPTS( x ); } while (0)

    #define portDISABLE_INTERRUPTS()                  ({                  \
       uint32_t ulState;                                                  \
       __asm volatile ("mrs %0, PRIMASK" : "=r" (ulState)::);             \
       __asm volatile ( " cpsid i " ::: "memory" );                       \
       ulState;})

    extern void vPortEnableInterrupts();
    #define portENABLE_INTERRUPTS()                   vPortEnableInterrupts()

    void vTaskEnterCritical(void);
    void vTaskExitCritical(void);
    #define portENTER_CRITICAL()                   vTaskEnterCritical()
    #define portEXIT_CRITICAL()                    vTaskExitCritical()

    #define portRTOS_SPINLOCK_COUNT 2

    /* Note this is a single method with uxAcquire parameter since we have
     * static vars, the method is always called with a compile time constant for
     * uxAcquire, and the compiler should dothe right thing! */
    #ifdef VERIFAST
        /* Reason for rewrite: VeriFast does not support local static variables.
         */
    #else
        static inline void vPortRecursiveLock(uint32_t ulLockNum, spin_lock_t *pxSpinLock, BaseType_t uxAcquire) {
            static uint8_t ucOwnedByCore[ portMAX_CORE_COUNT ];
            static uint8_t ucRecursionCountByLock[ portRTOS_SPINLOCK_COUNT ];
            configASSERT(ulLockNum >= 0 && ulLockNum < portRTOS_SPINLOCK_COUNT );
            uint32_t ulCoreNum = get_core_num();
            uint32_t ulLockBit = 1u << ulLockNum;
            configASSERT(ulLockBit < 256u );
            if( uxAcquire )
            {
                if( __builtin_expect( !*pxSpinLock, 0 ) )
                {
                    if( ucOwnedByCore[ulCoreNum] & ulLockBit )
                    {
                        configASSERT(ucRecursionCountByLock[ulLockNum] != 255u );
                        ucRecursionCountByLock[ulLockNum]++;
                        return;
                    }
                    while ( __builtin_expect( !*pxSpinLock, 0 ) );
                }
                __mem_fence_acquire();
                configASSERT(ucRecursionCountByLock[ulLockNum] == 0 );
                ucRecursionCountByLock[ulLockNum] = 1;
                ucOwnedByCore[ulCoreNum] |= ulLockBit;
            } else {
                configASSERT((ucOwnedByCore[ulCoreNum] & ulLockBit) != 0 );
                configASSERT(ucRecursionCountByLock[ulLockNum] != 0 );
                if( !--ucRecursionCountByLock[ulLockNum] )
                {
                    ucOwnedByCore[ulCoreNum] &= ~ulLockBit;
                    __mem_fence_release();
                    *pxSpinLock = 1;
                }
            }
        }
    #endif /* VERIFAST */

    #define portGET_ISR_LOCK()      vPortRecursiveLock(0, spin_lock_instance(configSMP_SPINLOCK_0), pdTRUE)
    #define portRELEASE_ISR_LOCK()  vPortRecursiveLock(0, spin_lock_instance(configSMP_SPINLOCK_0), pdFALSE)
    #define portGET_TASK_LOCK()     vPortRecursiveLock(1, spin_lock_instance(configSMP_SPINLOCK_1), pdTRUE)
    #define portRELEASE_TASK_LOCK() vPortRecursiveLock(1, spin_lock_instance(configSMP_SPINLOCK_1), pdFALSE)

/*-----------------------------------------------------------*/

/* Tickless idle/low power functionality. */
    #ifndef portSUPPRESS_TICKS_AND_SLEEP
        extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );
        #define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime )    vPortSuppressTicksAndSleep( xExpectedIdleTime )
    #endif
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
    #define portTASK_FUNCTION_PROTO( vFunction, pvParameters )    void vFunction( void * pvParameters )
    #define portTASK_FUNCTION( vFunction, pvParameters )          void vFunction( void * pvParameters )

    #define portNOP()

    #define portMEMORY_BARRIER()    __asm volatile ( "" ::: "memory" )

    #ifdef __cplusplus
        }
    #endif

#endif /* PORTMACRO_H */
