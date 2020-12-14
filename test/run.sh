#!/bin/bash
BASE_DIR=$(dirname "$0")
BIN_DIR=bin
pushd $BASE_DIR
mkdir -p $BIN_DIR
gcc -o $BIN_DIR/io_validation io_validation.c -lpthread -Wall -g
gcc -o $BIN_DIR/io_validation_spin_lock io_validation_spin_lock.c -lpthread -Wall -g
gcc -o $BIN_DIR/io_validation_destroy io_validation_destroy.c -lpthread -Wall -g
gcc -o $BIN_DIR/io_validation_fifo io_validation_fifo.c -lpthread -Wall -g
./$BIN_DIR/io_validation 1 1 16
./$BIN_DIR/io_validation 4 4 256
./$BIN_DIR/io_validation 128 128 131072
./$BIN_DIR/io_validation 1024 1024 1048576
./$BIN_DIR/io_validation 32 128 131072
./$BIN_DIR/io_validation 128 32 131072
./$BIN_DIR/io_validation 1 128 131072
./$BIN_DIR/io_validation 128 1 131072
./$BIN_DIR/io_validation_spin_lock 1 1 16
./$BIN_DIR/io_validation_spin_lock 4 4 256
./$BIN_DIR/io_validation_spin_lock 4 16 131072
./$BIN_DIR/io_validation_spin_lock 16 4 131072
./$BIN_DIR/io_validation_spin_lock 1 32 131072
./$BIN_DIR/io_validation_spin_lock 32 1 131072
./$BIN_DIR/io_validation_destroy 1 1
./$BIN_DIR/io_validation_destroy 2 2
./$BIN_DIR/io_validation_destroy 4 4
./$BIN_DIR/io_validation_destroy 8 8
./$BIN_DIR/io_validation_destroy 16 16
./$BIN_DIR/io_validation_destroy 32 32
./$BIN_DIR/io_validation_destroy 64 64
./$BIN_DIR/io_validation_destroy 256 256
./$BIN_DIR/io_validation_destroy 1024 1024
./$BIN_DIR/io_validation_destroy 2048 2048
./$BIN_DIR/io_validation_destroy 1 256
./$BIN_DIR/io_validation_destroy 256 1
./$BIN_DIR/io_validation_destroy 1 1024
./$BIN_DIR/io_validation_destroy 1024 1
./$BIN_DIR/io_validation_fifo 2 2
./$BIN_DIR/io_validation_fifo 4 4
./$BIN_DIR/io_validation_fifo 8 8
./$BIN_DIR/io_validation_fifo 16 16
./$BIN_DIR/io_validation_fifo 32 32
./$BIN_DIR/io_validation_fifo 128 128
popd
