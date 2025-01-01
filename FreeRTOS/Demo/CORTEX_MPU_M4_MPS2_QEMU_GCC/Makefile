CC                     =    arm-none-eabi-gcc
BIN                   :=    RTOSDemo.axf

BUILD_DIR             :=    build

FREERTOS_DIR_REL      :=    ../../../FreeRTOS
FREERTOS_DIR          :=    $(abspath $(FREERTOS_DIR_REL))
KERNEL_DIR            :=    $(FREERTOS_DIR)/Source


SOURCE_FILES          +=    $(KERNEL_DIR)/portable/GCC/ARM_CM4_MPU/port.c
SOURCE_FILES          +=    $(KERNEL_DIR)/portable/Common/mpu_wrappers.c
SOURCE_FILES          +=    $(KERNEL_DIR)/tasks.c
SOURCE_FILES          +=    $(KERNEL_DIR)/list.c
SOURCE_FILES          +=    $(KERNEL_DIR)/queue.c
SOURCE_FILES          +=    $(KERNEL_DIR)/timers.c
SOURCE_FILES          +=    $(KERNEL_DIR)/event_groups.c
SOURCE_FILES          +=    $(KERNEL_DIR)/stream_buffer.c
SOURCE_FILES          +=    ${KERNEL_DIR}/portable/MemMang/heap_4.c

INCLUDE_DIRS          +=    -I.
INCLUDE_DIRS          +=    -I$(KERNEL_DIR)/include
INCLUDE_DIRS          +=    -I$(KERNEL_DIR)/portable/GCC/ARM_CM4_MPU

SOURCE_FILES          +=    main.c
SOURCE_FILES          +=    app_main.c
SOURCE_FILES          +=    mpu_demo.c
SOURCE_FILES          +=    startup.c
SOURCE_FILES          +=    syscall.c


DEFINES               :=    -DQEMU_SOC_MPS2
DEFINES               :=    -DHEAP4
DEFINES               +=    -DportUSING_MPU_WRAPPERS=1

CPPFLAGS              +=    $(DEFINES)

CFLAGS                +=    -mfloat-abi=softfp
CFLAGS                +=    -mcpu=cortex-m4
CFLAGS                +=    -Wno-error=implicit-function-declaration
CFLAGS                +=    -Wno-builtin-declaration-mismatch
CFLAGS                +=    -Werror
CFLAGS                +=    -Wall
CFLAGS                +=    -Wextra
CFLAGS                +=    -fstrict-aliasing
CFLAGS                +=    -Wstrict-aliasing
CFLAGS                +=    -Wno-error=address-of-packed-member
CFLAGS                +=    $(INCLUDE_DIRS)
CFLAGS                +=    -fno-builtin
CFLAGS                +=    -ggdb3 -Og -save-temps=obj

LDFLAGS                =    -T mps2_m4.ld
LDFLAGS               +=    -mthumb
LDFLAGS               +=    -Xlinker -Map=${BUILD_DIR}/output.map
LDFLAGS               +=    -nostartfiles
LDFLAGS               +=    -specs=nosys.specs
LDFLAGS               +=    -fno-builtin

OBJ_FILES             :=    $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)

.PHONY: clean

$(BUILD_DIR)/$(BIN) : $(OBJ_FILES)
	$(CC) -ffunction-sections -fdata-sections $(LDFLAGS) $+ -o $(@)

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

INCLUDES              :=     $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.d)
-include $(INCLUDES)

${BUILD_DIR}/%.o : %.c Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -c $< -o $@

${BUILD_DIR}/%.o : %.S Makefile
	-mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	-rm -rf build

