/*
* copyright (c) 2006-2016 ARM Limited
* SPDX-License-Identifier: BSD-3-Clause
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* File:     smm_mps2.h
* Release:  Version 1.1
*******************************************************************************/

#ifndef __SMM_MPS2_H
#define __SMM_MPS2_H

#include "CMSDK_CM3.h"                         /* device specific header file    */

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/******************************************************************************/
/*                          FPGA System Register declaration                  */
/******************************************************************************/

typedef struct                  
{
  __IO uint32_t LED;             // Offset: 0x000 (R/W)  LED connections
                                 //                         [31:2] : Reserved
                                 //                          [1:0] : LEDs
       uint32_t RESERVED1[1];
  __IO uint32_t BUTTON;          // Offset: 0x008 (R/W)  Buttons
                                 //                         [31:2] : Reserved
                                 //                          [1:0] : Buttons
       uint32_t RESERVED2[1];
  __IO uint32_t CLK1HZ;          // Offset: 0x010 (R/W)  1Hz up counter
  __IO uint32_t CLK100HZ;        // Offset: 0x014 (R/W)  100Hz up counter
  __IO uint32_t COUNTER;         // Offset: 0x018 (R/W)  Cycle Up Counter
                                 //                         Increments when 32-bit prescale counter reach zero
       uint32_t RESERVED3[1];
  __IO uint32_t PRESCALE;        // Offset: 0x020 (R/W)  Prescaler
                                 //                         Bit[31:0] : reload value for prescale counter
  __IO uint32_t PSCNTR;          // Offset: 0x024 (R/W)  32-bit Prescale counter
                                 //                         current value of the pre-scaler counter
                                 //                         The Cycle Up Counter increment when the prescale down counter reach 0
                                 //                         The pre-scaler counter is reloaded with PRESCALE after reaching 0.
       uint32_t RESERVED4[9];
  __IO uint32_t MISC;            // Offset: 0x04C (R/W)  Misc control */
                                 //                         [31:10] : Reserved
                                 //                            [9] : SHIELD_1_SPI_nCS
                                 //                            [8] : SHIELD_0_SPI_nCS
                                 //                            [7] : ADC_SPI_nCS
                                 //                            [6] : CLCD_BL_CTRL
                                 //                            [5] : CLCD_RD
                                 //                            [4] : CLCD_RS
                                 //                            [3] : CLCD_RESET
                                 //                            [2] : RESERVED
                                 //                            [1] : SPI_nSS
                                 //                            [0] : CLCD_CS
} MPS2_FPGAIO_TypeDef;

// MISC register bit definitions

#define CLCD_CS_Pos        0
#define CLCD_CS_Msk        (1UL<<CLCD_CS_Pos)
#define SPI_nSS_Pos        1
#define SPI_nSS_Msk        (1UL<<SPI_nSS_Pos)
#define CLCD_RESET_Pos     3
#define CLCD_RESET_Msk     (1UL<<CLCD_RESET_Pos)
#define CLCD_RS_Pos        4
#define CLCD_RS_Msk        (1UL<<CLCD_RS_Pos)
#define CLCD_RD_Pos        5
#define CLCD_RD_Msk        (1UL<<CLCD_RD_Pos)
#define CLCD_BL_Pos        6
#define CLCD_BL_Msk        (1UL<<CLCD_BL_Pos)
#define ADC_nCS_Pos        7
#define ADC_nCS_Msk        (1UL<<ADC_nCS_Pos)
#define SHIELD_0_nCS_Pos        8
#define SHIELD_0_nCS_Msk        (1UL<<SHIELD_0_nCS_Pos)
#define SHIELD_1_nCS_Pos        9
#define SHIELD_1_nCS_Msk        (1UL<<SHIELD_1_nCS_Pos)

/******************************************************************************/
/*                        SCC Register declaration                            */
/******************************************************************************/

typedef struct                   //
{
  __IO uint32_t CFG_REG0;        // Offset: 0x000 (R/W)  Remaps block RAM to ZBT
                                 //                         [31:1] : Reserved
                                 //                            [0] 1 : REMAP BlockRam to ZBT
  __IO uint32_t LEDS;            // Offset: 0x004 (R/W)  Controls the MCC user LEDs
                                 //                         [31:8] : Reserved
                                 //                          [7:0] : MCC LEDs
       uint32_t RESERVED0[1];
  __I  uint32_t SWITCHES;        // Offset: 0x00C (R/ )  Denotes the state of the MCC user switches
                                 //                         [31:8] : Reserved
                                 //                          [7:0] : These bits indicate state of the MCC switches
  __I  uint32_t CFG_REG4;        // Offset: 0x010 (R/ )  Denotes the board revision
                                 //                         [31:4] : Reserved
                                 //                          [3:0] : Used by the MCC to pass PCB revision. 0 = A 1 = B
       uint32_t RESERVED1[35];
  __IO uint32_t SYS_CFGDATA_RTN; // Offset: 0x0A0 (R/W)  User data register
                                 //                         [31:0] : Data
  __IO uint32_t SYS_CFGDATA_OUT; // Offset: 0x0A4 (R/W)  User data register
                                 //                         [31:0] : Data
  __IO uint32_t SYS_CFGCTRL;     // Offset: 0x0A8 (R/W)  Control register
                                 //                           [31] : Start (generates interrupt on write to this bit)
                                 //                           [30] : R/W access
                                 //                        [29:26] : Reserved
                                 //                        [25:20] : Function value
                                 //                        [19:12] : Reserved
                                 //                         [11:0] : Device (value of 0/1/2 for supported clocks)
  __IO uint32_t SYS_CFGSTAT;     // Offset: 0x0AC (R/W)  Contains status information
                                 //                         [31:2] : Reserved
                                 //                            [1] : Error
                                 //                            [0] : Complete
  __IO uint32_t RESERVED2[20];
  __IO uint32_t SCC_DLL;         // Offset: 0x100 (R/W)  DLL Lock Register
                                 //                        [31:24] : DLL LOCK MASK[7:0] - Indicate if the DLL locked is masked
                                 //                        [23:16] : DLL LOCK MASK[7:0] - Indicate if the DLLs are locked or unlocked
                                 //                         [15:1] : Reserved
                                 //                            [0] : This bit indicates if all enabled DLLs are locked
       uint32_t RESERVED3[957];
  __I  uint32_t SCC_AID;         // Offset: 0xFF8 (R/ )  SCC AID Register
                                 //                        [31:24] : FPGA build number
                                 //                        [23:20] : V2M-MPS2 target board revision (A = 0, B = 1)
                                 //                        [19:11] : Reserved
                                 //                           [10] : if “1” SCC_SW register has been implemented
                                 //                            [9] : if “1” SCC_LED register has been implemented
                                 //                            [8] : if “1” DLL lock register has been implemented
                                 //                          [7:0] : number of SCC configuration register
  __I  uint32_t SCC_ID;          // Offset: 0xFFC (R/ )  Contains information about the FPGA image
                                 //                        [31:24] : Implementer ID: 0x41 = ARM
                                 //                        [23:20] : Application note IP variant number
                                 //                        [19:16] : IP Architecture: 0x4 =AHB
                                 //                         [15:4] : Primary part number: 386 = AN386
                                 //                          [3:0] : Application note IP revision number
} MPS2_SCC_TypeDef;


/******************************************************************************/
/*                        SSP Peripheral declaration                          */
/******************************************************************************/

typedef struct                   // Document DDI0194G_ssp_pl022_r1p3_trm.pdf
{
  __IO uint32_t CR0;             // Offset: 0x000 (R/W)  Control register 0
                                 //                        [31:16] : Reserved
                                 //                         [15:8] : Serial clock rate
                                 //                            [7] : SSPCLKOUT phase,    applicable to Motorola SPI frame format only
                                 //                            [6] : SSPCLKOUT polarity, applicable to Motorola SPI frame format only
                                 //                          [5:4] : Frame format
                                 //                          [3:0] : Data Size Select
  __IO uint32_t CR1;             // Offset: 0x004 (R/W)  Control register 1
                                 //                         [31:4] : Reserved
                                 //                            [3] : Slave-mode output disable
                                 //                            [2] : Master or slave mode select
                                 //                            [1] : Synchronous serial port enable
                                 //                            [0] : Loop back mode
  __IO uint32_t DR;              // Offset: 0x008 (R/W)  Data register
                                 //                        [31:16] : Reserved
                                 //                         [15:0] : Transmit/Receive FIFO
  __I  uint32_t SR;              // Offset: 0x00C (R/ )  Status register
                                 //                         [31:5] : Reserved
                                 //                            [4] : PrimeCell SSP busy flag
                                 //                            [3] : Receive FIFO full
                                 //                            [2] : Receive FIFO not empty
                                 //                            [1] : Transmit FIFO not full
                                 //                            [0] : Transmit FIFO empty
  __IO uint32_t CPSR;            // Offset: 0x010 (R/W)  Clock prescale register
                                 //                         [31:8] : Reserved
                                 //                          [8:0] : Clock prescale divisor
  __IO uint32_t IMSC;            // Offset: 0x014 (R/W)  Interrupt mask set or clear register
                                 //                         [31:4] : Reserved
                                 //                            [3] : Transmit FIFO interrupt mask
                                 //                            [2] : Receive FIFO interrupt mask
                                 //                            [1] : Receive timeout interrupt mask
                                 //                            [0] : Receive overrun interrupt mask
  __I  uint32_t RIS;             // Offset: 0x018 (R/ )  Raw interrupt status register
                                 //                         [31:4] : Reserved
                                 //                            [3] : raw interrupt state, prior to masking, of the SSPTXINTR interrupt
                                 //                            [2] : raw interrupt state, prior to masking, of the SSPRXINTR interrupt
                                 //                            [1] : raw interrupt state, prior to masking, of the SSPRTINTR interrupt
                                 //                            [0] : raw interrupt state, prior to masking, of the SSPRORINTR interrupt
  __I  uint32_t MIS;             // Offset: 0x01C (R/ )  Masked interrupt status register
                                 //                         [31:4] : Reserved
                                 //                            [3] : transmit FIFO masked interrupt state, after masking, of the SSPTXINTR interrupt
                                 //                            [2] : receive FIFO masked interrupt state, after masking, of the SSPRXINTR interrupt
                                 //                            [1] : receive timeout masked interrupt state, after masking, of the SSPRTINTR interrupt
                                 //                            [0] : receive over run masked interrupt status, after masking, of the SSPRORINTR interrupt
  __O  uint32_t ICR;             // Offset: 0x020 ( /W)  Interrupt clear register
                                 //                         [31:2] : Reserved
                                 //                            [1] : Clears the SSPRTINTR interrupt
                                 //                            [0] : Clears the SSPRORINTR interrupt
  __IO uint32_t DMACR;           // Offset: 0x024 (R/W)  DMA control register
                                 //                         [31:2] : Reserved
                                 //                            [1] : Transmit DMA Enable
                                 //                            [0] : Receive DMA Enable
} MPS2_SSP_TypeDef;


// SSP_CR0 Control register 0
#define SSP_CR0_DSS_Pos         0           // Data Size Select
#define SSP_CR0_DSS_Msk         (0xF<<SSP_CR0_DSS_Pos)
#define SSP_CR0_FRF_Pos         4           // Frame Format Select
#define SSP_CR0_FRF_Msk         (3UL<<SSP_CR0_FRM_Pos)
#define SSP_CR0_SPO_Pos         6           // SSPCLKOUT polarity
#define SSP_CR0_SPO_Msk         (1UL<<SSP_CR0_SPO_Pos)
#define SSP_CR0_SPH_Pos         7           // SSPCLKOUT phase
#define SSP_CR0_SPH_Msk         (1UL<<SSP_CR0_SPH_Pos)
#define SSP_CR0_SCR_Pos         8           // Serial Clock Rate (divide)
#define SSP_CR0_SCR_Msk         (0xFF<<SSP_CR0_SCR_Pos)

#define SSP_CR0_SCR_DFLT        0x0300      // Serial Clock Rate (divide), default set at 3
#define SSP_CR0_FRF_MOT         0x0000      // Frame format, Motorola
#define SSP_CR0_DSS_8           0x0007      // Data packet size, 8bits
#define SSP_CR0_DSS_16          0x000F      // Data packet size, 16bits

// SSP_CR1 Control register 1
#define SSP_CR1_LBM_Pos         0           // Loop Back Mode
#define SSP_CR1_LBM_Msk         (1UL<<SSP_CR1_LBM_Pos)
#define SSP_CR1_SSE_Pos         1           // Serial port enable
#define SSP_CR1_SSE_Msk         (1UL<<SSP_CR1_SSE_Pos)
#define SSP_CR1_MS_Pos          2           // Master or Slave mode
#define SSP_CR1_MS_Msk          (1UL<<SSP_CR1_MS_Pos)
#define SSP_CR1_SOD_Pos         3           // Slave Output mode Disable
#define SSP_CR1_SOD_Msk         (1UL<<SSP_CR1_SOD_Pos)

// SSP_SR Status register
#define SSP_SR_TFE_Pos          0           // Transmit FIFO empty
#define SSP_SR_TFE_Msk          (1UL<<SSP_SR_TFE_Pos)
#define SSP_SR_TNF_Pos          1           // Transmit FIFO not full
#define SSP_SR_TNF_Msk          (1UL<<SSP_SR_TNF_Pos)
#define SSP_SR_RNE_Pos          2           // Receive  FIFO not empty
#define SSP_SR_RNE_Msk          (1UL<<SSP_SR_RNE_Pos)
#define SSP_SR_RFF_Pos          3           // Receive  FIFO full
#define SSP_SR_RFF_Msk          (1UL<<SSP_SR_RFF_Pos)
#define SSP_SR_BSY_Pos          4           // Busy
#define SSP_SR_BSY_Msk          (1UL<<SSP_SR_BSY_Pos)

// SSP_CPSR Clock prescale register
#define SSP_CPSR_CPD_Pos        0           // Clock prescale divisor
#define SSP_CPSR_CPD_Msk        (0xFF<<SSP_CPSR_CDP_Pos)

#define SSP_CPSR_DFLT        0x0008      // Clock prescale (use with SCR), default set at 8

// SSPIMSC Interrupt mask set and clear register
#define SSP_IMSC_RORIM_Pos         0           // Receive overrun not Masked
#define SSP_IMSC_RORIM_Msk         (1UL<<SSP_IMSC_RORIM_Pos)
#define SSP_IMSC_RTIM_Pos          1           // Receive timeout not Masked
#define SSP_IMSC_RTIM_Msk          (1UL<<SSP_IMSC_RTIM_Pos)
#define SSP_IMSC_RXIM_Pos          2           // Receive  FIFO not Masked
#define SSP_IMSC_RXIM_Msk          (1UL<<SSP_IMSC_RXIM_Pos)
#define SSP_IMSC_TXIM_Pos          3           // Transmit FIFO not Masked
#define SSP_IMSC_TXIM_Msk          (1UL<<SSP_IMSC_TXIM_Pos)

// SSPRIS Raw interrupt status register
#define SSP_RIS_RORRIS_Pos         0           // Raw Overrun  interrupt flag
#define SSP_RIS_RORRIS_Msk         (1UL<<SSP_RIS_RORRIS_Pos)
#define SSP_RIS_RTRIS_Pos          1           // Raw Timemout interrupt flag
#define SSP_RIS_RTRIS_Msk          (1UL<<SSP_RIS_RTRIS_Pos)
#define SSP_RIS_RXRIS_Pos          2           // Raw Receive  interrupt flag
#define SSP_RIS_RXRIS_Msk          (1UL<<SSP_RIS_RXRIS_Pos)
#define SSP_RIS_TXRIS_Pos          3           // Raw Transmit interrupt flag
#define SSP_RIS_TXRIS_Msk          (1UL<<SSP_RIS_TXRIS_Pos)

// SSPMIS Masked interrupt status register
#define SSP_MIS_RORMIS_Pos         0           // Masked Overrun  interrupt flag
#define SSP_MIS_RORMIS_Msk         (1UL<<SSP_MIS_RORMIS_Pos)
#define SSP_MIS_RTMIS_Pos          1           // Masked Timemout interrupt flag
#define SSP_MIS_RTMIS_Msk          (1UL<<SSP_MIS_RTMIS_Pos)
#define SSP_MIS_RXMIS_Pos          2           // Masked Receive  interrupt flag
#define SSP_MIS_RXMIS_Msk          (1UL<<SSP_MIS_RXMIS_Pos)
#define SSP_MIS_TXMIS_Pos          3           // Masked Transmit interrupt flag
#define SSP_MIS_TXMIS_Msk          (1UL<<SSP_MIS_TXMIS_Pos)

// SSPICR Interrupt clear register
#define SSP_ICR_RORIC_Pos           0           // Clears Overrun  interrupt flag
#define SSP_ICR_RORIC_Msk           (1UL<<SSP_ICR_RORIC_Pos)
#define SSP_ICR_RTIC_Pos            1           // Clears Timemout interrupt flag
#define SSP_ICR_RTIC_Msk            (1UL<<SSP_ICR_RTIC_Pos)

// SSPDMACR DMA control register
#define SSP_DMACR_RXDMAE_Pos        0           // Enable Receive  FIFO DMA
#define SSP_DMACR_RXDMAE_Msk        (1UL<<SSP_DMACR_RXDMAE_Pos)
#define SSP_DMACR_TXDMAE_Pos        1           // Enable Transmit FIFO DMA
#define SSP_DMACR_TXDMAE_Msk        (1UL<<SSP_DMACR_TXDMAE_Pos)

/******************************************************************************/
/*               Audio and Touch Screen (I2C) Peripheral declaration          */
/******************************************************************************/

typedef struct
{
  union {
  __O   uint32_t  CONTROLS;     // Offset: 0x000 CONTROL Set Register     ( /W)
  __I   uint32_t  CONTROL;      // Offset: 0x000 CONTROL Status Register  (R/ )
  };
  __O    uint32_t  CONTROLC;     // Offset: 0x004 CONTROL Clear Register    ( /W) 
} MPS2_I2C_TypeDef;

#define SDA                1 << 1
#define SCL                1 << 0


/******************************************************************************/
/*               Audio I2S Peripheral declaration                             */
/******************************************************************************/

typedef struct
{
  /*!< Offset: 0x000 CONTROL Register    (R/W) */
  __IO   uint32_t  CONTROL; // <h> CONTROL </h>
                              //   <o.0> TX Enable
                              //     <0=> TX disabled
                              //     <1=> TX enabled
                              //   <o.1> TX IRQ Enable
                              //     <0=> TX IRQ disabled
                              //     <1=> TX IRQ enabled
                              //   <o.2> RX Enable
                              //     <0=> RX disabled
                              //     <1=> RX enabled
                              //   <o.3> RX IRQ Enable
                              //     <0=> RX IRQ disabled
                              //     <1=> RX IRQ enabled
                              //   <o.10..8> TX Buffer Water Level
                              //     <0=> / IRQ triggers when any space available
                              //     <1=> / IRQ triggers when more than 1 space available
                              //     <2=> / IRQ triggers when more than 2 space available
                              //     <3=> / IRQ triggers when more than 3 space available
                              //     <4=> Undefined!
                              //     <5=> Undefined!
                              //     <6=> Undefined!
                              //     <7=> Undefined! 
                              //   <o.14..12> RX Buffer Water Level
                              //     <0=> Undefined!
                              //     <1=> / IRQ triggers when less than 1 space available
                              //     <2=> / IRQ triggers when less than 2 space available
                              //     <3=> / IRQ triggers when less than 3 space available
                              //     <4=> / IRQ triggers when less than 4 space available
                              //     <5=> Undefined!
                              //     <6=> Undefined!
                              //     <7=> Undefined! 
                              //   <o.16> FIFO reset
                              //     <0=> Normal operation
                              //     <1=> FIFO reset
                              //   <o.17> Audio Codec reset
                              //     <0=> Normal operation
                              //     <1=> Assert audio Codec reset
  /*!< Offset: 0x004 STATUS Register     (R/ ) */
  __I    uint32_t  STATUS;  // <h> STATUS </h>
                              //   <o.0> TX Buffer alert
                              //     <0=> TX buffer don't need service yet
                              //     <1=> TX buffer need service
                              //   <o.1> RX Buffer alert
                              //     <0=> RX buffer don't need service yet
                              //     <1=> RX buffer need service
                              //   <o.2> TX Buffer Empty
                              //     <0=> TX buffer have data
                              //     <1=> TX buffer empty
                              //   <o.3> TX Buffer Full
                              //     <0=> TX buffer not full
                              //     <1=> TX buffer full
                              //   <o.4> RX Buffer Empty
                              //     <0=> RX buffer have data
                              //     <1=> RX buffer empty
                              //   <o.5> RX Buffer Full
                              //     <0=> RX buffer not full
                              //     <1=> RX buffer full
  union {
   /*!< Offset: 0x008 Error Status Register (R/ ) */ 
    __I    uint32_t  ERROR;  // <h> ERROR </h>
                              //   <o.0> TX error
                              //     <0=> Okay
                              //     <1=> TX overrun/underrun
                              //   <o.1> RX error
                              //     <0=> Okay
                              //     <1=> RX overrun/underrun
   /*!< Offset: 0x008 Error Clear Register  ( /W) */   
    __O    uint32_t  ERRORCLR;  // <h> ERRORCLR </h>
                              //   <o.0> TX error
                              //     <0=> Okay
                              //     <1=> Clear TX error
                              //   <o.1> RX error
                              //     <0=> Okay
                              //     <1=> Clear RX error
    };
   /*!< Offset: 0x00C Divide ratio Register (R/W) */ 
  __IO   uint32_t  DIVIDE;  // <h> Divide ratio for Left/Right clock </h>
                              //   <o.9..0> TX error (default 0x80)                 
   /*!< Offset: 0x010 Transmit Buffer       ( /W) */
  __O    uint32_t  TXBUF;  // <h> Transmit buffer </h>
                              //   <o.15..0> Right channel                 
                              //   <o.31..16> Left channel
   /*!< Offset: 0x014 Receive Buffer        (R/ ) */
  __I    uint32_t  RXBUF;  // <h> Receive buffer </h>
                              //   <o.15..0> Right channel                 
                              //   <o.31..16> Left channel
         uint32_t  RESERVED1[186];
  __IO uint32_t ITCR;         // <h> Integration Test Control Register </h>
                              //   <o.0> ITEN
                              //     <0=> Normal operation
                              //     <1=> Integration Test mode enable
  __O  uint32_t ITIP1;        // <h> Integration Test Input Register 1</h>
                              //   <o.0> SDIN
  __O  uint32_t ITOP1;        // <h> Integration Test Output Register 1</h>
                              //   <o.0> SDOUT
                              //   <o.1> SCLK
                              //   <o.2> LRCK
                              //   <o.3> IRQOUT
} MPS2_I2S_TypeDef;

#define I2S_CONTROL_TXEN_Pos        0
#define I2S_CONTROL_TXEN_Msk        (1UL<<I2S_CONTROL_TXEN_Pos)

#define I2S_CONTROL_TXIRQEN_Pos     1
#define I2S_CONTROL_TXIRQEN_Msk     (1UL<<I2S_CONTROL_TXIRQEN_Pos)

#define I2S_CONTROL_RXEN_Pos        2
#define I2S_CONTROL_RXEN_Msk        (1UL<<I2S_CONTROL_RXEN_Pos)

#define I2S_CONTROL_RXIRQEN_Pos     3
#define I2S_CONTROL_RXIRQEN_Msk     (1UL<<I2S_CONTROL_RXIRQEN_Pos)

#define I2S_CONTROL_TXWLVL_Pos      8
#define I2S_CONTROL_TXWLVL_Msk      (7UL<<I2S_CONTROL_TXWLVL_Pos)

#define I2S_CONTROL_RXWLVL_Pos      12
#define I2S_CONTROL_RXWLVL_Msk      (7UL<<I2S_CONTROL_RXWLVL_Pos)
/* FIFO reset*/
#define I2S_CONTROL_FIFORST_Pos     16
#define I2S_CONTROL_FIFORST_Msk     (1UL<<I2S_CONTROL_FIFORST_Pos)
/* Codec reset*/
#define I2S_CONTROL_CODECRST_Pos    17
#define I2S_CONTROL_CODECRST_Msk    (1UL<<I2S_CONTROL_CODECRST_Pos)

#define I2S_STATUS_TXIRQ_Pos        0
#define I2S_STATUS_TXIRQ_Msk        (1UL<<I2S_STATUS_TXIRQ_Pos)

#define I2S_STATUS_RXIRQ_Pos        1
#define I2S_STATUS_RXIRQ_Msk        (1UL<<I2S_STATUS_RXIRQ_Pos)

#define I2S_STATUS_TXEmpty_Pos      2
#define I2S_STATUS_TXEmpty_Msk      (1UL<<I2S_STATUS_TXEmpty_Pos)

#define I2S_STATUS_TXFull_Pos       3
#define I2S_STATUS_TXFull_Msk       (1UL<<I2S_STATUS_TXFull_Pos)

#define I2S_STATUS_RXEmpty_Pos      4
#define I2S_STATUS_RXEmpty_Msk      (1UL<<I2S_STATUS_RXEmpty_Pos)

#define I2S_STATUS_RXFull_Pos       5
#define I2S_STATUS_RXFull_Msk       (1UL<<I2S_STATUS_RXFull_Pos)

#define I2S_ERROR_TXERR_Pos         0
#define I2S_ERROR_TXERR_Msk         (1UL<<I2S_ERROR_TXERR_Pos)

#define I2S_ERROR_RXERR_Pos         1
#define I2S_ERROR_RXERR_Msk         (1UL<<I2S_ERROR_RXERR_Pos)

/******************************************************************************/
/*                       SMSC9220 Register Definitions                        */
/******************************************************************************/

typedef struct                   // SMSC LAN9220
{
__I   uint32_t  RX_DATA_PORT;          //   Receive FIFO Ports (offset 0x0)
      uint32_t  RESERVED1[0x7];
__O   uint32_t  TX_DATA_PORT;          //   Transmit FIFO Ports (offset 0x20)
      uint32_t  RESERVED2[0x7];

__I   uint32_t  RX_STAT_PORT;          //   Receive FIFO status port (offset 0x40)
__I   uint32_t  RX_STAT_PEEK;          //   Receive FIFO status peek (offset 0x44)
__I   uint32_t  TX_STAT_PORT;          //   Transmit FIFO status port (offset 0x48)
__I   uint32_t  TX_STAT_PEEK;          //   Transmit FIFO status peek (offset 0x4C)

__I   uint32_t  ID_REV;                //   Chip ID and Revision (offset 0x50)
__IO  uint32_t  IRQ_CFG;               //   Main Interrupt Configuration (offset 0x54)
__IO  uint32_t  INT_STS;               //   Interrupt Status (offset 0x58)
__IO  uint32_t  INT_EN;                //   Interrupt Enable Register (offset 0x5C)
      uint32_t  RESERVED3;             //   Reserved for future use (offset 0x60)
__I   uint32_t  BYTE_TEST;             //   Read-only byte order testing register 87654321h (offset 0x64)
__IO  uint32_t  FIFO_INT;              //   FIFO Level Interrupts (offset 0x68)
__IO  uint32_t  RX_CFG;                //   Receive Configuration (offset 0x6C)
__IO  uint32_t  TX_CFG;                //   Transmit Configuration (offset 0x70)
__IO  uint32_t  HW_CFG;                //   Hardware Configuration (offset 0x74)
__IO  uint32_t  RX_DP_CTL;             //   RX Datapath Control (offset 0x78)
__I   uint32_t  RX_FIFO_INF;           //   Receive FIFO Information (offset 0x7C)
__I   uint32_t  TX_FIFO_INF;           //   Transmit FIFO Information (offset 0x80)
__IO  uint32_t  PMT_CTRL;              //   Power Management Control (offset 0x84)
__IO  uint32_t  GPIO_CFG;              //   General Purpose IO Configuration (offset 0x88)
__IO  uint32_t  GPT_CFG;               //   General Purpose Timer Configuration (offset 0x8C)
__I   uint32_t  GPT_CNT;               //   General Purpose Timer Count (offset 0x90)
      uint32_t  RESERVED4;             //   Reserved for future use (offset 0x94)
__IO  uint32_t  ENDIAN;                //   WORD SWAP Register (offset 0x98)
__I   uint32_t  FREE_RUN;              //   Free Run Counter (offset 0x9C)
__I   uint32_t  RX_DROP;               //   RX Dropped Frames Counter (offset 0xA0)
__IO  uint32_t  MAC_CSR_CMD;           //   MAC CSR Synchronizer Command (offset 0xA4)
__IO  uint32_t  MAC_CSR_DATA;          //   MAC CSR Synchronizer Data (offset 0xA8)
__IO  uint32_t  AFC_CFG;               //   Automatic Flow Control Configuration (offset 0xAC)
__IO  uint32_t  E2P_CMD;               //   EEPROM Command (offset 0xB0)
__IO  uint32_t  E2P_DATA;              //   EEPROM Data (offset 0xB4)

} SMSC9220_TypeDef;

// SMSC9220 MAC Registers       Indices
#define SMSC9220_MAC_CR         0x1
#define SMSC9220_MAC_ADDRH      0x2
#define SMSC9220_MAC_ADDRL      0x3
#define SMSC9220_MAC_HASHH      0x4
#define SMSC9220_MAC_HASHL      0x5
#define SMSC9220_MAC_MII_ACC    0x6
#define SMSC9220_MAC_MII_DATA   0x7
#define SMSC9220_MAC_FLOW       0x8
#define SMSC9220_MAC_VLAN1      0x9
#define SMSC9220_MAC_VLAN2      0xA
#define SMSC9220_MAC_WUFF       0xB
#define SMSC9220_MAC_WUCSR      0xC

// SMSC9220 PHY Registers       Indices
#define SMSC9220_PHY_BCONTROL   0x0
#define SMSC9220_PHY_BSTATUS    0x1
#define SMSC9220_PHY_ID1        0x2
#define SMSC9220_PHY_ID2        0x3
#define SMSC9220_PHY_ANEG_ADV   0x4
#define SMSC9220_PHY_ANEG_LPA   0x5
#define SMSC9220_PHY_ANEG_EXP   0x6
#define SMSC9220_PHY_MCONTROL   0x17
#define SMSC9220_PHY_MSTATUS    0x18
#define SMSC9220_PHY_CSINDICATE 0x27
#define SMSC9220_PHY_INTSRC     0x29
#define SMSC9220_PHY_INTMASK    0x30
#define SMSC9220_PHY_CS         0x31

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

#define MPS2_SSP1_BASE          (0x40020000ul)       /* User SSP Base Address   */
#define MPS2_SSP0_BASE          (0x40021000ul)       /* CLCD SSP Base Address   */
#define MPS2_TSC_I2C_BASE       (0x40022000ul)       /* Touch Screen I2C Base Address */
#define MPS2_AAIC_I2C_BASE      (0x40023000ul)       /* Audio Interface I2C Base Address */
#define MPS2_AAIC_I2S_BASE      (0x40024000ul)       /* Audio Interface I2S Base Address */
#define MPS2_SSP2_BASE          (0x40025000ul)       /* adc SSP Base Address   */
#define MPS2_SSP3_BASE          (0x40026000ul)       /* Shield 0 SSP Base Address   */
#define MPS2_SSP4_BASE          (0x40027000ul)       /* Shield 1 SSP Base Address   */
#define MPS2_FPGAIO_BASE        (0x40028000ul)       /* FPGAIO Base Address */
#define MPS2_SHIELD0_I2C_BASE   (0x40029000ul)       /* Shield 0 I2C Base Address */
#define MPS2_SHIELD1_I2C_BASE   (0x4002A000ul)       /* Shield 1 I2C Base Address */
#define MPS2_SCC_BASE           (0x4002F000ul)       /* SCC Base Address    */

#ifdef CORTEX_M7
#define SMSC9220_BASE           (0xA0000000ul)       /* Ethernet SMSC9220 Base Address   */
#else
#define SMSC9220_BASE           (0x40200000ul)       /* Ethernet SMSC9220 Base Address   */
#endif

#define MPS2_VGA_TEXT_BUFFER    (0x41000000ul)       /* VGA Text Buffer Address */
#define MPS2_VGA_BUFFER         (0x41100000ul)       /* VGA Buffer Base Address */

/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/

#define SMSC9220                ((SMSC9220_TypeDef      *) SMSC9220_BASE )
#define MPS2_TS_I2C             ((MPS2_I2C_TypeDef      *) MPS2_TSC_I2C_BASE )
#define MPS2_AAIC_I2C           ((MPS2_I2C_TypeDef      *) MPS2_AAIC_I2C_BASE )
#define MPS2_SHIELD0_I2C        ((MPS2_I2C_TypeDef      *) MPS2_SHIELD0_I2C_BASE )
#define MPS2_SHIELD1_I2C        ((MPS2_I2C_TypeDef      *) MPS2_SHIELD1_I2C_BASE )
#define MPS2_AAIC_I2S           ((MPS2_I2S_TypeDef      *) MPS2_AAIC_I2S_BASE )
#define MPS2_FPGAIO             ((MPS2_FPGAIO_TypeDef   *) MPS2_FPGAIO_BASE )
#define MPS2_SCC                ((MPS2_SCC_TypeDef      *) MPS2_SCC_BASE )
#define MPS2_SSP0               ((MPS2_SSP_TypeDef      *) MPS2_SSP0_BASE )
#define MPS2_SSP1               ((MPS2_SSP_TypeDef      *) MPS2_SSP1_BASE )
#define MPS2_SSP2               ((MPS2_SSP_TypeDef      *) MPS2_SSP2_BASE )    
#define MPS2_SSP3               ((MPS2_SSP_TypeDef      *) MPS2_SSP3_BASE )    
#define MPS2_SSP4               ((MPS2_SSP_TypeDef      *) MPS2_SSP4_BASE )    

/******************************************************************************/
/*                     General Function Definitions                           */
/******************************************************************************/


/******************************************************************************/
/*                     General MACRO Definitions                              */
/******************************************************************************/



#endif /* __SMM_MPS2_H */
