#!/bin/bash
make main

./main.exe 2&> 1.log &
./main.exe 2&> 2.log &
./main.exe 2&> 3.log &
./main.exe 2&> 4.log &

./main.exe 2&> 5.log &
./main.exe 2&> 6.log &
./main.exe 2&> 7.log &
./main.exe 2&> 8.log &

./main.exe 2&> 9.log &
./main.exe 2&> 10.log &
./main.exe 2&> 11.log &
./main.exe 2&> 12.log &

./main.exe 2&> 13.log &
./main.exe 2&> 14.log &
./main.exe 2&> 15.log &
./main.exe 2&> 16.log &
