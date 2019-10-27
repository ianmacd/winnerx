################################################################################
1. How to Build
        - get Toolchain
                From android git serveru, codesourcery and etc ..
                - gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-

                From OSRC (http://opensource.samsung.com/)
                - keyword : llvm-arm-toolchain-ship
                  : llvm-arm-toolchain-ship/6.0/bin/clang , llvm-arm-toolchain-ship/6.0-cfp/bin/clang

        - make output folder 
                EX) OUTPUT_DIR=out
                $ mkdir out

        - edit Makefile
                edit "CROSS_COMPILE" to right toolchain path(You downloaded).
                        EX)  CROSS_COMPILE=<android platform directory you download>/android/prebuilts/gcc-cfp/gcc-cfp-single/aarch64-linux-android-4.9/bin/aarch64-linux-android-
                        Ex)  CROSS_COMPILE=/usr/local/toolchain/gcc-cfp/gcc-cfp-single/aarch64-linux-android-4.9/bin/aarch64-linux-android- // check the location of toolchain
                edit "REAL_CC" to right toolchain path(You downloaded).
                        EX)  CC=<android platform directory you download>/android/vendor/qcom/proprietary/llvm-arm-toolchain-ship/6.0/bin/clang
                edit "CLANG_TRIPLE" to right path(You downloaded).
                        EX) CLANG_TRIPLE=aarch64-linux-gnu-
                edit "CFP_CC" to right toolchain path(You downloaded).
                        EX) CFP_CC=<android platform directory you download>/android/vendor/qcom/proprietary/llvm-arm-toolchain-ship/6.0-cfp/bin/clang

        - to Build
                $ export ARCH=arm64
                $ make -C $(pwd) O=$(pwd)/out DTC_EXT=$(pwd)/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y CLANG_TRIPLE=aarch64-linux-gnu- winnerx_eur_open_defconfig
                $ make -C $(pwd) O=$(pwd)/out DTC_EXT=$(pwd)/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y CLANG_TRIPLE=aarch64-linux-gnu-


2. Output files
        - Kernel : arch/arm64/boot/Image
        - module : drivers/*/*.ko

3. How to Clean
        Change to OUTPUT_DIR folder
        EX) /home/dpi/qb5_8814/workspace/P4_1716/android/out/target/product/gts6l/out
        $ make clean
################################################################################
