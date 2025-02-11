# MQTT Mutual Authentication on QEMU-emulated MPS2 Cortex M3 AN385

## Requirements

1. GNU Arm Embedded Toolchain download [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
1. qemu-system-arm download [here](https://www.qemu.org/download)
1. cmake

## How to download

Navigate to a directory of your choice and run the following command:
```
$ git clone https://github.com/FreeRTOS/FreeRTOS.git --recurse-submodules --depth 1
```
The previous command should create a directory named **FreeRTOS**. The demo will be located in `<path-to-where-you-ran-git-clone>/FreeRTOS/FreeRTOS-Plus/Demo/MQTT_Mutual_Auth_Qemu_mps2`.

## MQTT Mutual Auth

The demo will work with any mTLS-enabled MQTT broker. The demo can be configured to use username/password authentication or X.509 authentication with client certificates.

The instructions below is specifically for the AWS IoT Core MQTT broker using X.509 authentication. Check `include/demo_config.h` for other authentication methods.

### Connecting with AWS IoT Core

To connect to the AWS IoT Core, a few steps are necessary; see the steps below.

#### Building and Running

##### Create a Thing in the AWS IoT Core

1. Go to aws.amazon.com and login or create an account:
1. Navigate to AWS IoT Core and in the left menu, click on Manage -> All devices -> Things
1. Click "Create things" and then create a single thing.
1. Give the thing a name, and select 'no device shadow'
1. Save the Endpoint, Root CA, Client Cert. and Client Private key

##### Fill defines values (see below) in `include/demo_config.h`

1. Ensure to include the headers and footers and to format as shown in the comments for each macro.
1. For client identifier, insert the name of the thing you created in the last step.
```c
#define democonfigCLIENT_IDENTIFIER
#define democonfigMQTT_BROKER_ENDPOINT
#define democonfigROOT_CA_PEM
#define democonfigCLIENT_CERTIFICATE_PEM
#define democonfigCLIENT_PRIVATE_KEY_PEM
```

##### Build your software

```sh
$ mkdir build && cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE=../arm-gcc-toolchain.cmake
```
options (add in front of `cmake`): DEBUG=1 to build with **-O0** and debugging symbols

##### Run the demo
```sh
$ qemu-system-arm -machine mps2-an385 -cpu cortex-m3 \
          -kernel ./build/FreeRTOS-MQTTS-MutualAuth-Demo-QEMU-MPS2 \
          -netdev user,id=mynet0 \
          -net nic,macaddr=52:54:00:12:34:AD,model=lan9118,netdev=mynet0 \
          -display none -m 16M -nographic -serial stdio \
          -monitor null -semihosting -semihosting-config enable=on,target=native
```

##### Expectations

1. Navigate to AWS IoT Core and click on "MQTT Test Client"
1. In the topic section, enter "#" (message on all topics)
1. With the demo running, you should see "Hello World!" messages in the client

## How to start debugging
We use the `gdb` in the GNU Arm Embedded Toolchain (that is included in the requirements)

Append the `-s` and `-S` switches to the previous demo-running command (`qemu-system-arm`):
* `-s`: allow gdb to be attached to the process remotely at port 1234
* `-S`: start the program in the paused state

run: (make sure you build the debug version)
```
$ arm-none-eabi-gdb -q ./build/RTOSDemo.axf

(gdb) target remote :1234
(gdb) break main
(gdb) c
```

## Note on the Entropy Source

Within mbedtls_freertos_port.c, mbedtls_platform_entropy_poll utilizes a pseudo-random number
generator. This allows MPS2-AN385 boards emulated via QEMU to establish a TLS connection, but should
be updated if you plan to build a project off of this demo to improve security.
