cmake_minimum_required(VERSION 3.22.1)

project(FreeRTOS-MQTTS-MutualAuth-Demo-QEMU-MPS2 C)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR} CACHE STRING "")
set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_path(CONVERT "${CMAKE_CURRENT_SOURCE_DIR}" TO_CMAKE_PATH_LIST DEMO_DIR NORMALIZE)
cmake_path(SET FREERTOS_DIR NORMALIZE "${DEMO_DIR}/../../..")
cmake_path(SET FREERTOS_KERNEL_DIR NORMALIZE "${FREERTOS_DIR}/FreeRTOS/Source")
cmake_path(SET FREERTOS_PLUS_DIR NORMALIZE "${FREERTOS_DIR}/FreeRTOS-Plus")
cmake_path(SET FREERTOS_PLUS_TCP_DIR NORMALIZE "${FREERTOS_PLUS_DIR}/Source/FreeRTOS-Plus-TCP")
cmake_path(SET MBEDTLS_DIR NORMALIZE "${FREERTOS_PLUS_DIR}/ThirdParty/mbedtls")
cmake_path(SET MBEDTLS_WRAPPER_DIR NORMALIZE "${FREERTOS_PLUS_DIR}/Source/Application-Protocols/network_transport")
cmake_path(SET COREMQTT_DIR NORMALIZE "${FREERTOS_PLUS_DIR}/Source/Application-Protocols/coreMQTT")
cmake_path(SET UTILITIES_DIR NORMALIZE "${FREERTOS_PLUS_DIR}/Source/Utilities")

# cmake_path(NATIVE_PATH FREERTOS_DIR NORMALIZE FREERTOS_DIR)
# cmake_path(NATIVE_PATH FREERTOS_KERNEL_DIR NORMALIZE FREERTOS_KERNEL_DIR)
# cmake_path(NATIVE_PATH FREERTOS_PLUS_DIR NORMALIZE FREERTOS_PLUS_DIR)
# cmake_path(NATIVE_PATH FREERTOS_PLUS_TCP_DIR NORMALIZE FREERTOS_PLUS_TCP_DIR)
# cmake_path(NATIVE_PATH MBEDTLS_DIR NORMALIZE MBEDTLS_DIR)
# cmake_path(NATIVE_PATH MBEDTLS_WRAPPER_DIR NORMALIZE MBEDTLS_WRAPPER_DIR)
# cmake_path(NATIVE_PATH COREMQTT_DIR NORMALIZE COREMQTT_DIR)
# cmake_path(NATIVE_PATH UTILITIES_DIR NORMALIZE UTILITIES_DIR)

file(GLOB DEMO_SOURCES
	source/*.c

	${FREERTOS_KERNEL_DIR}/*.c

	${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM3/port.c
	${FREERTOS_KERNEL_DIR}/portable/MemMang/heap_3.c

	${FREERTOS_PLUS_TCP_DIR}/source/*.c

	${FREERTOS_PLUS_TCP_DIR}/source/portable/BufferManagement/BufferAllocation_2.c
	${FREERTOS_PLUS_TCP_DIR}/source/portable/NetworkInterface/MPS2_AN385/NetworkInterface.c
	${FREERTOS_PLUS_TCP_DIR}/source/portable/NetworkInterface/MPS2_AN385/ether_lan9118/smsc9220_eth_drv.c

	${MBEDTLS_DIR}/library/*.c

	${MBEDTLS_WRAPPER_DIR}/transport_mbedtls.c
	${MBEDTLS_WRAPPER_DIR}/mbedtls_bio_tcp_sockets_wrapper.c
	${MBEDTLS_WRAPPER_DIR}/tcp_sockets_wrapper/ports/freertos_plus_tcp/sockets_wrapper.c
	${MBEDTLS_WRAPPER_DIR}/tcp_sockets_wrapper/ports/freertos_plus_tcp/tcp_sockets_wrapper.c

	${FREERTOS_PLUS_DIR}/VisualStudio_StaticProjects/MbedTLS/mbedtls_freertos_port.c

	${COREMQTT_DIR}/source/*.c

	${UTILITIES_DIR}/backoff_algorithm/source/backoff_algorithm.c
)

add_definitions(-DprojCOVERAGE_TEST -DQEMU_SOC_MPS2 -DHEAP3)
add_definitions(-DMBEDTLS_CONFIG_FILE="${CMAKE_CURRENT_SOURCE_DIR}/include/demo_mbedtls_config.h")
add_definitions(-DmainCREATE_NETWORKING_DEMO_ONLY=1)
add_definitions(-DmainCREATE_MQTT_TASKS_SINGLE=1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles -mthumb -mcpu=cortex-m3 -Wno-error=implicit-function-declaration")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-builtin-declaration-mismatch -Werror")
if(DEFINED $ENV{DEBUG})
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3 -Og")
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstrict-aliasing -Wstrict-aliasing -Wno-error=address-of-packed-member")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/source/mps2_m3.ld -specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=rdimon.specs -lc -lrdimon -Xlinker -Map=${PROJECT_BINARY_DIR}/output.map")

add_executable(${PROJECT_NAME} ${DEMO_SOURCES})
target_include_directories(${PROJECT_NAME}
	PUBLIC include

	PUBLIC include/CMSIS

	PUBLIC ${FREERTOS_KERNEL_DIR}/include

	PUBLIC ${FREERTOS_KERNEL_DIR}/portable/GCC/ARM_CM3

	PUBLIC ${FREERTOS_PLUS_TCP_DIR}/source/include

	PUBLIC ${FREERTOS_PLUS_TCP_DIR}/source/portable/Compiler/GCC
	PUBLIC ${FREERTOS_PLUS_TCP_DIR}/source/portable/NetworkInterface/MPS2_AN385/ether_lan9118

	PUBLIC ${MBEDTLS_DIR}/library
	PUBLIC ${MBEDTLS_DIR}/include

	PUBLIC ${MBEDTLS_WRAPPER_DIR}
	PUBLIC ${MBEDTLS_WRAPPER_DIR}/tcp_sockets_wrapper/include
	PUBLIC ${MBEDTLS_WRAPPER_DIR}/tcp_sockets_wrapper/ports/freertos_plus_tcp

	PUBLIC ${FREERTOS_PLUS_DIR}/VisualStudio_StaticProjects/MbedTLS

	PUBLIC ${COREMQTT_DIR}/source/include
	PUBLIC ${COREMQTT_DIR}/source/interface

	PUBLIC ${UTILITIES_DIR}/logging
	PUBLIC ${UTILITIES_DIR}/backoff_algorithm/source/include
)
