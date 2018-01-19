#!/bin/bash
NDK=/Users/jxy/android-ndk-r11c
SYSROOT=${NDK}/platforms/android-21/arch-arm
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64
CPU=arm
PREFIX=/Users/jxy/android/sipphone/baresip-android/ffmpeg
ADD_CFLAGS="-marm"
ADD_LDFLAGS=""

echo "./configure \
--prefix=$PREFIX \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--target-os=linux \
--arch=arm \
--enable-cross-compile \
--sysroot=$SYSROOT \
--extra-cflags='-Os -fpic $ADD_CFLAGS' \
--extra-ldflags='$ADD_LDFLAGS' \
$ADDITIONAL_CONFIGURE_FLAG"

./configure \
--prefix=$PREFIX \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--target-os=linux \
--arch=arm \
--enable-cross-compile \
--sysroot=$SYSROOT \
--extra-cflags='-Os -fpic $ADD_CFLAGS' \
--extra-ldflags='$ADD_LDFLAGS' \
$ADDITIONAL_CONFIGURE_FLAG
