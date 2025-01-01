/*
 * FreeRTOS V202212.00
 * Copyright (C) 2023 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See https://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#define configASSERT_DEFINED 1
extern void vAssertCalled( void );
#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( )

#define configUSE_PREEMPTION        1
#define configUSE_TIME_SLICING      1

#define configUSE_IDLE_HOOK         0
#define configUSE_TICK_HOOK         0
#define configCPU_CLOCK_HZ        ( ( unsigned long ) 20000000 )
#define configTICK_RATE_HZ        ( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE  ( ( unsigned short ) 2000 )
#define configTOTAL_HEAP_SIZE     ( ( size_t ) ( 279000 ) )
#define configMAX_TASK_NAME_LEN   ( 10 )
#define configUSE_TRACE_FACILITY    0
#define configUSE_16_BIT_TICKS      0
#define configIDLE_SHOULD_YIELD     0
#define configUSE_CO_ROUTINES       0

#define configMAX_PRIORITIES            ( 10 )
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )
#define configTIMER_QUEUE_LENGTH          20
#define configTIMER_TASK_PRIORITY       ( configMAX_PRIORITIES - 3 )
#define configUSE_COUNTING_SEMAPHORES 1
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configSUPPORT_STATIC_ALLOCATION 1
#define  configNUM_TX_DESCRIPTORS 15

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define configUSE_MALLOC_FAILED_HOOK    1
#define configUSE_MUTEXES               1
#define configUSE_RECURSIVE_MUTEXES     1
#define INCLUDE_vTaskPrioritySet        0
#define INCLUDE_uxTaskPriorityGet       0
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskCleanUpResources   0
#define INCLUDE_vTaskSuspend            0
#define INCLUDE_vTaskDelayUntil         1
#define INCLUDE_vTaskDelay              1

#define configKERNEL_INTERRUPT_PRIORITY         252
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    25 /* equivalent to 0xa0, or priority 5. */
#define configMAC_INTERRUPT_PRIORITY 2

/* networking definitions */
#define configMAC_ISR_SIMULATOR_PRIORITY     ( configMAX_PRIORITIES - 2 )
#define ipconfigUSE_NETWORK_EVENT_HOOK 1
//#define ipconfigSOCK_DEFAULT_RECEIVE_BLOCK_TIME  pdMS_TO_TICKS(5000)
#define configNETWORK_INTERFACE_TO_USE 1L


/* Default MAC address configuration.  The demo creates a virtual network
connection that uses this MAC address by accessing the raw Ethernet/WiFi data
to and from a real network connection on the host PC.  See the
configNETWORK_INTERFACE_TO_USE definition above for information on how to
configure the real network connection to use. */

#define configMAC_ADDR0    0x52
#define configMAC_ADDR1    0x54
#define configMAC_ADDR2    0x00
#define configMAC_ADDR3    0x12
#define configMAC_ADDR4    0x34
#define configMAC_ADDR5    0xAD

/* Default IP address configuration.  Used in ipconfigUSE_DNS is set to 0, or
ipconfigUSE_DNS is set to 1 but a DNS server cannot be contacted. */

#define configIP_ADDR0      10
#define configIP_ADDR1      211
#define configIP_ADDR2      55
#define configIP_ADDR3      5

/* Default gateway IP address configuration.  Used in ipconfigUSE_DNS is set to
0, or ipconfigUSE_DNS is set to 1 but a DNS server cannot be contacted. */

#define configGATEWAY_ADDR0 10
#define configGATEWAY_ADDR1 211
#define configGATEWAY_ADDR2 55
#define configGATEWAY_ADDR3 5

/* Default DNS server configuration.  OpenDNS addresses are used if ipconfigUSE_DNS is set
to 1 but a DNS server cannot be contacted.*/

#define configDNS_SERVER_ADDR0  127
#define configDNS_SERVER_ADDR1  0
#define configDNS_SERVER_ADDR2  0
#define configDNS_SERVER_ADDR3  53

/* Default netmask configuration.  Used in ipconfigUSE_DNS is set to 0, or
ipconfigUSE_DNS is set to 1 but a DNS server cannot be contacted. */
#define configNET_MASK0     255
#define configNET_MASK1     255
#define configNET_MASK2     255
#define configNET_MASK3     0

/* The UDP port to which print messages are sent. */
#define configPRINT_PORT   (15000)
#endif /* FREERTOS_CONFIG_H */
