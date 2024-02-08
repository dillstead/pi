# pi
Miscellaneous Raspberry Pi Bare Metal Projects

To compile the projects, you first need to install a toolchain.  You can download the toolchain [here](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).

After you install the toolchain, make sure to set the `ARM_TOOLCHAIN` environment variable to point to your the toolchain's installation directory:
```
ARM_TOOLCHAIN=<path to your toolchain>/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi
```
