################################################################################

1. Download and unzip the kernel source of F907BXXU3BTDD.

2. Unzip and update the kernel source of F907BXXS4BTF2.

3. How to Build
        - get Toolchain
                From android git serveru, codesourcery and etc ..
                - gcc-cfp/gcc-cfp-single/aarch64-linux-android-4.9/bin/aarch64-linux-android-

                From Qualcomm developer network (https://developer.qualcomm.com/software/snapdragon-llvm-compiler-android/tools)
                - llvm-arm-toolchain-ship/8.0/
                
        - make output folder 
                EX) OUTPUT_DIR=out
                $ mkdir out

        - edit Makefile
                edit "CROSS_COMPILE" to right toolchain path(You downloaded).
                        EX)  CROSS_COMPILE=<android platform directory you download>/android/prebuilts/gcc-cfp/gcc-cfp-single/aarch64-linux-android-4.9/bin/aarch64-linux-android-
                        Ex)  CROSS_COMPILE=/usr/local/toolchain/gcc-cfp/gcc-cfp-single/aarch64-linux-android-4.9/bin/aarch64-linux-android- // check the location of toolchain
                edit "REAL_CC" to right toolchain path(You downloaded).
                        EX)  CC=<android platform directory you download>/android/vendor/qcom/proprietary/llvm-arm-toolchain-ship/8.0/bin/clang
                edit "CLANG_TRIPLE" to right path(You downloaded).

        - to Build
                $ export ARCH=arm64
                $ make -C $(pwd) O=$(pwd)/out DTC_EXT=$(pwd)/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y CLANG_TRIPLE=aarch64-linux-gnu- winnerlte_eur_open_defconfig
                $ make -C $(pwd) O=$(pwd)/out DTC_EXT=$(pwd)/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y CLANG_TRIPLE=aarch64-linux-gnu-


4. Output files
        - Kernel : arch/arm64/boot/Image
        - module : drivers/*/*.ko

5. How to Clean
        Change to OUTPUT_DIR folder
        EX) /home/dpi/qb5_8814/workspace/P4_1716/android/out/target/product/winnerx/out
        $ make clean
################################################################################
