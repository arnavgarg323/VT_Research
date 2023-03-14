# aes-riscv
This project contains the Bernstein's cache timing side-channel attack for RISC-V platform.

# How to compile and build the source code

Download the code: `git clone https://github.com/salmanyam/aes-riscv.git`

We can compile and build the code for both x86 and RISC platforms simply using the `make` command. However, before issuing the `make` command we need to change the toolchain and portion of the code in `server.c` file.

Use the the following lines to set the compiler toolchains for compiling the code for x86 platform.
```
CC= gcc
AR= gcc-ar r
```

Use the the following lines to set the compiler toolchains for compiling the code for RISCV platform. Please use the appropriate directory where you have installed the rocket-tools.
```
CC= /home/danfeng/cache/installs/riscv/toolchain/bin/riscv64-unknown-elf-gcc
AR= /home/danfeng/cache/installs/riscv/toolchain/bin/riscv64-unknown-elf-gcc-ar r
```

There are two implementations for the `unsigned int timestamp(void)` function: one for x86 and another for RISCV. Please use the appropriate one before compiling and building the code.

Please use the following implementation of the `unsigned int timestamp(void)` function for the x86 platform.
```
// for x86 platform
unsigned int timestamp(void) {
        unsigned int bottom;
        unsigned int top;
        asm volatile(".byte 15;.byte 49": "=a"(bottom), "=d"(top));
        return bottom;
}
```

Please the following implementation of the `unsigned int timestamp(void)` function for the RISCV platform.
```
//For RISCV platform
unsigned int timestamp(void) {
        unsigned long cycles;
    asm volatile ("rdcycle %0" : "=r" (cycles));
    return cycles;
}
```

Once we have made the two changes, we can simply issue the `make` command to compile and build the source code.


# How to run the program

Once we have compiled and built the program, we will see three programs: `server`, `correlate`, and `search`. The `server` program is responsbile for generating the various timing information using cache hits and misses. To generate the timing information, the run the `server` program as follows.

```
./server <packet size> < <secret key>
Here, packet size is the size of the packet that the AES algorithm uses to encrypt and 
secret key is the key for AES. An example parameters is as follows:

./server 20 < /dev/zero
```
Running the server using the abovementioned way will output the timing information in the terminal. We can redirect the output to a file as follows:
```
./server 20 < /dev/zero > output.txt
```

