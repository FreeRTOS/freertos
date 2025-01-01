/*
 * FreeRTOS V202212.00
 * Copyright (C) 2023 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

/*
 * This project is a cut down version of the project described on the following
 * link.  Only the simple UDP client and server and the TCP echo clients are
 * included in the build:
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/examples_FreeRTOS_simulator.html
 */

/* Standard includes. */
#include <stdio.h>
#include <time.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include "task.h"

/* Demo application includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/* Ethernet device driver includes */
#include "smsc9220_eth_drv.h"
#include "SMM_MPS2.h"
#include "CMSDK_CM3.h"


/* Simple UDP client and server task parameters. */
#define mainSIMPLE_UDP_CLIENT_SERVER_TASK_PRIORITY    ( tskIDLE_PRIORITY )
#define mainSIMPLE_UDP_CLIENT_SERVER_PORT             ( 5005UL )

/* Echo client task parameters - used for both TCP and UDP echo clients. */
#define mainECHO_CLIENT_TASK_STACK_SIZE               ( configMINIMAL_STACK_SIZE * 2 )      /* Not used in the linux port. */
#define mainECHO_CLIENT_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 1 )

/* Echo server task parameters. */
#define mainECHO_SERVER_TASK_STACK_SIZE               ( configMINIMAL_STACK_SIZE * 2 )      /* Not used in the linux port. */
#define mainECHO_SERVER_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 1 )

/* Define a name that will be used for LLMNR and NBNS searches. */
#define mainHOST_NAME                                 "RTOSDemo"
#define mainDEVICE_NICK_NAME                          "qemu_demo"

extern void vStartSimpleMQTTDemo( void );

/* Set the following constants to 1 or 0 to define which tasks to include and
 * exclude:
 *
 * mainCREATE_MQTT_TASKS_SINGLE:  When set to 1 a set of tasks are created that
 * create a MQTT connection on top of a mutual TLS connection and publish messages 
 * to the MQTT broker defined in demo_config.h. */ 

/*-----------------------------------------------------------*/

/*
 * Just seeds the simple pseudo random number generator.
 */
static void prvSRand( UBaseType_t ulSeed );

/*
 * Miscellaneous initialisation including preparing the logging and seeding the
 * random number generator.
 */
static void prvMiscInitialisation( void );

/* The default IP and MAC address used by the demo.  The address configuration
 * defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is
 * 1 but a DHCP server could not be contacted.  See the online documentation for
 * more information. */
static const uint8_t ucIPAddress[ 4 ] = { configIP_ADDR0,
                                          configIP_ADDR1,
                                          configIP_ADDR2,
                                          configIP_ADDR3
                                        };
static const uint8_t ucNetMask[ 4 ] = { configNET_MASK0,
                                        configNET_MASK1,
                                        configNET_MASK2,
                                        configNET_MASK3
                                      };
static const uint8_t ucGatewayAddress[ 4 ] = { configGATEWAY_ADDR0,
                                               configGATEWAY_ADDR1,
                                               configGATEWAY_ADDR2,
                                               configGATEWAY_ADDR3
                                             };
static const uint8_t ucDNSServerAddress[ 4 ] = { configDNS_SERVER_ADDR0,
                                                 configDNS_SERVER_ADDR1,
                                                 configDNS_SERVER_ADDR2,
                                                 configDNS_SERVER_ADDR3
                                               };
const uint8_t ucMACAddress[ 6 ] = { configMAC_ADDR0,
                                    configMAC_ADDR1,
                                    configMAC_ADDR2,
                                    configMAC_ADDR3,
                                    configMAC_ADDR4,
                                    configMAC_ADDR5
                                  };

/* Use by the pseudo random number generator. */
static UBaseType_t ulNextRand;

/* Initially set to pdFALSE since tasks that use the IP stack have not 
 * yet been created, and then set to pdTRUE once they have been created. */
BaseType_t xTasksAlreadyCreated = pdFALSE;

/*-----------------------------------------------------------*/

/* Use by the pseudo random number generator. */
static UBaseType_t ulNextRand;

/*-----------------------------------------------------------*/

static void prvWait_ms( uint32_t ulSleep_ms )
{
    vTaskDelay( pdMS_TO_TICKS( ulSleep_ms ) );
}

/*-----------------------------------------------------------*/

static BaseType_t prvReadMacAddrFromEeprom( uint8_t * pucMACAddress )
{
    BaseType_t xResult = pdFALSE;

    static const struct smsc9220_eth_dev_cfg_t SMSC9220_ETH_DEV_CFG =
    {
        .base = SMSC9220_BASE
    };

    static struct smsc9220_eth_dev_data_t SMSC9220_ETH_DEV_DATA =
    {
        .state = 0
    };

    static const struct smsc9220_eth_dev_t SMSC9220_ETH_DEV =
    {
        &( SMSC9220_ETH_DEV_CFG ),
        &( SMSC9220_ETH_DEV_DATA )
    };

    enum smsc9220_error_t err;

    err = smsc9220_init( &SMSC9220_ETH_DEV, prvWait_ms );

    if( err == SMSC9220_ERROR_NONE )
    {
        /* Read MAC address from LAN9118 / 9220 */
        err = smsc9220_read_mac_address( &SMSC9220_ETH_DEV, ( char * ) pucMACAddress );
    }

    if( err == SMSC9220_ERROR_NONE )
    {
        xResult = pdTRUE;
    }

    return xResult;
}

/*-----------------------------------------------------------*/

void vPlatformInitIpStack( void )
{
    BaseType_t xResult;
    uint8_t ucIPAddress[ 4 ];
    uint8_t ucNullAddress[ 4 ] = { 0, 0, 0, 0 };
    uint8_t ucMACAddress[ 6 ];
    uint8_t ucDNSServerAddress[ 4 ];
    uint8_t ucGatewayAddress[ 4 ];

    FreeRTOS_printf( ( "in vPlatformInitIpStack\n" ) );

    UBaseType_t uxRandomNumber = uxRand();

    NVIC_SetPriority( ETHERNET_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY );

    /* Attempt to read MAC address from LAN9220 / LAN9118 eeprom */
    if( prvReadMacAddrFromEeprom( ucMACAddress ) == pdTRUE )
    {
        FreeRTOS_printf( ( "Using eeprom MAC address: %.02X:%.02X:%.02X:%.02X:%.02X:%.02X",
                    ucMACAddress[ 0 ], ucMACAddress[ 1 ], ucMACAddress[ 2 ],
                    ucMACAddress[ 3 ], ucMACAddress[ 4 ], ucMACAddress[ 5 ] ) );
    }
    else
    {
        /* Read MAC address from FreeRTOSConfig */
        ucMACAddress[ 0 ] = configMAC_ADDR0;
        ucMACAddress[ 1 ] = configMAC_ADDR1;
        ucMACAddress[ 2 ] = configMAC_ADDR2;
        ucMACAddress[ 3 ] = configMAC_ADDR3;
        ucMACAddress[ 4 ] = configMAC_ADDR4;
        ucMACAddress[ 5 ] = configMAC_ADDR5;

        FreeRTOS_printf( ( "Using random MAC address: %.02X:%.02X:%.02X:%.02X:%.02X:%.02X",
                    ucMACAddress[ 0 ], ucMACAddress[ 1 ], ucMACAddress[ 2 ],
                    ucMACAddress[ 3 ], ucMACAddress[ 4 ], ucMACAddress[ 5 ] ) );
    }

    ucIPAddress[ 0 ] = configIP_ADDR0;
    ucIPAddress[ 1 ] = configIP_ADDR1;
    ucIPAddress[ 2 ] = configIP_ADDR2;
    ucIPAddress[ 3 ] = configIP_ADDR3;

    ucDNSServerAddress[ 0 ] = configDNS_SERVER_ADDR0;
    ucDNSServerAddress[ 1 ] = configDNS_SERVER_ADDR1;
    ucDNSServerAddress[ 2 ] = configDNS_SERVER_ADDR2;
    ucDNSServerAddress[ 3 ] = configDNS_SERVER_ADDR3;

    ucGatewayAddress[ 0 ] = configGATEWAY_ADDR0;
    ucGatewayAddress[ 1 ] = configGATEWAY_ADDR1;
    ucGatewayAddress[ 2 ] = configGATEWAY_ADDR2;
    ucGatewayAddress[ 3 ] = configGATEWAY_ADDR3;

    xResult = FreeRTOS_IPInit( ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress );
    configASSERT( xResult == pdTRUE );
}

void vStartupTask( void* pvParameters )
{
    const uint32_t ulLongTime_ms = pdMS_TO_TICKS( 1000UL );

    vPlatformInitIpStack();

    vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/

/* Called by FreeRTOS+TCP when the network connects or disconnects.  Disconnect
 * events are only received if implemented in the MAC driver. */
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    uint32_t ulIPAddress;
    uint32_t ulNetMask;
    uint32_t ulGatewayAddress;
    uint32_t ulDNSServerAddress;
    char cBuffer[ 16 ];

    /* If the network has just come up...*/
    if( eNetworkEvent == eNetworkUp )
    {
        /* Create the tasks that use the IP stack if they have not already been
         * created. */
        if( xTasksAlreadyCreated == pdFALSE )
        {
            /* See the comments above the definitions of these pre-processor
             * macros at the top of this file for a description of the individual
             * demo tasks. */

            #if ( mainCREATE_MQTT_TASKS_SINGLE == 1 )
                {
                    vStartSimpleMQTTDemo();
                }
            #endif /* mainCREATE_TCP_ECHO_TASKS_SINGLE */

            xTasksAlreadyCreated = pdTRUE;
        }

        /* Print out the network configuration, which may have come from a DHCP
         * server. */
        FreeRTOS_GetAddressConfiguration( &ulIPAddress, &ulNetMask, &ulGatewayAddress, &ulDNSServerAddress );
        FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
        FreeRTOS_printf( ( "\r\n\r\nIP Address: %s\r\n", cBuffer ) );

        FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
        FreeRTOS_printf( ( "Subnet Mask: %s\r\n", cBuffer ) );

        FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
        FreeRTOS_printf( ( "Gateway Address: %s\r\n", cBuffer ) );

        FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
        FreeRTOS_printf( ( "DNS Server Address: %s\r\n\r\n\r\n", cBuffer ) );
    }
    else
    {
        FreeRTOS_printf( ("Application idle hook network down\n") );
    }
}
/*-----------------------------------------------------------*/

UBaseType_t uxRand( void )
{
    const uint32_t ulMultiplier = 0x015a4e35UL, ulIncrement = 1UL;

    /* Utility function to generate a pseudo random number. */

    ulNextRand = ( ulMultiplier * ulNextRand ) + ulIncrement;
    return( ( int ) ( ulNextRand >> 16UL ) & 0x7fffUL );
}
/*-----------------------------------------------------------*/

static void prvSRand( UBaseType_t ulSeed )
{
    /* Utility function to seed the pseudo random number generator. */
    ulNextRand = ulSeed;
}
/*-----------------------------------------------------------*/

static void prvMiscInitialisation( void )
{
    time_t xTimeNow;

    /* Seed the random number generator. */
    time( &xTimeNow );
    FreeRTOS_debug_printf( ( "Seed for randomiser: %lu\n", xTimeNow ) );
    prvSRand( ( uint32_t ) xTimeNow );
    FreeRTOS_debug_printf( ( "Random numbers: %08X %08X %08X %08X\n",
                             ipconfigRAND32(),
                             ipconfigRAND32(),
                             ipconfigRAND32(),
                             ipconfigRAND32() ) );
}
/*-----------------------------------------------------------*/

#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )

    const char * pcApplicationHostnameHook( void )
    {
        /* Assign the name "FreeRTOS" to this network node.  This function will
         * be called during the DHCP: the machine will be registered with an IP
         * address plus this name. */
        return mainHOST_NAME;
    }

#endif
/*-----------------------------------------------------------*/

#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )

    BaseType_t xApplicationDNSQueryHook( const char * pcName )
    {
        BaseType_t xReturn;

        /* Determine if a name lookup is for this node.  Two names are given
         * to this node: that returned by pcApplicationHostnameHook() and that set
         * by mainDEVICE_NICK_NAME. */
        if( strcasecmp( pcName, pcApplicationHostnameHook() ) == 0 )
        {
            xReturn = pdPASS;
        }
        else if( strcasecmp( pcName, mainDEVICE_NICK_NAME ) == 0 )
        {
            xReturn = pdPASS;
        }
        else
        {
            xReturn = pdFAIL;
        }

        return xReturn;
    }

#endif /* if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) */

/*
 * Callback that provides the inputs necessary to generate a randomized TCP
 * Initial Sequence Number per RFC 6528.  THIS IS ONLY A DUMMY IMPLEMENTATION
 * THAT RETURNS A PSEUDO RANDOM NUMBER SO IS NOT INTENDED FOR USE IN PRODUCTION
 * SYSTEMS.
 */
extern uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                                    uint16_t usSourcePort,
                                                    uint32_t ulDestinationAddress,
                                                    uint16_t usDestinationPort )
{
    ( void ) ulSourceAddress;
    ( void ) usSourcePort;
    ( void ) ulDestinationAddress;
    ( void ) usDestinationPort;

    return uxRand();
}

/*
 * Supply a random number to FreeRTOS+TCP stack.
 * THIS IS ONLY A DUMMY IMPLEMENTATION THAT RETURNS A PSEUDO RANDOM NUMBER
 * SO IS NOT INTENDED FOR USE IN PRODUCTION SYSTEMS.
 */
BaseType_t xApplicationGetRandomNumber( uint32_t * pulNumber )
{
    *( pulNumber ) = uxRand();
    return pdTRUE;
}
