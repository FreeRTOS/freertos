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

MEMORY
{
    FLASH (xr) : ORIGIN = 0x00000000, LENGTH = 4M /* to 0x00003FFF = 0x007FFFFF*/
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 4M /* to 0x21FFFFFF = 0xFFFFFF */
}
ENTRY(Reset_Handler)

_Min_Heap_Size = 0x40000 ;        /* Required amount of heap. */
_Min_Stack_Size = 0x4000 ;       /* Required amount of stack. */
M_VECTOR_RAM_SIZE = (16 + 48) * 4;
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{

    .isr_vector :
    {
        __vector_table = .;
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text*)
        KEEP (*(.init))
        KEEP (*(.fini))
        KEEP(*(.eh_frame))
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

	.ARM.extab :
	{
		. = ALIGN(4);
		*(.ARM.extab* .gnu.linkonce.armextab.*)
		. = ALIGN(4);
	} >FLASH

	.ARM :
	{
		. = ALIGN(4);
		__exidx_start = .;
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
		__exidx_end = .;
		. = ALIGN(4);
	} >FLASH

    .interrupts_ram :
    {
        . = ALIGN(4);
        __VECTOR_RAM__ = .;
        __interrupts_ram_start__ = .;
        . += M_VECTOR_RAM_SIZE;
        . = ALIGN(4);
        __interrupts_ram_end = .;

    } > RAM

    _sidata = LOADADDR(.data);

    .data : /* AT ( _sidata ) */
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    .uninitialized (NOLOAD):
    {
        . = ALIGN(32);
        __uninitialized_start = .;
        *(.uninitialized)
        KEEP(*(.keep.uninitialized))
        . = ALIGN(32);
        __uninitialized_end = .;
    } > RAM

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } >RAM

    .heap :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        _heap_bottom = .;
        . = . + _Min_Heap_Size;
        _heap_top = .;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
       } >RAM

   /* Set stack top to end of RAM, and stack limit move down by
    * size of stack_dummy section */
   __StackTop = ORIGIN(RAM) + LENGTH(RAM);
   __StackLimit = __StackTop - _Min_Stack_Size;
   PROVIDE(__stack = __StackTop);
     
  /* Check if data + heap + stack exceeds RAM limit */
  ASSERT(__StackLimit >= _heap_top, "region RAM overflowed with stack")
}

