# !/usr/bin/bash

./sim bimodal 7 0 0 gcc_trace.txt
./sim bimodal 10 0 0 gcc_trace.txt
./sim bimodal 5 0 0 jpeg_trace.txt
./sim bimodal 6 0 0 perl_trace.txt

./sim gshare 10 4 0 0 gcc_trace.txt
./sim gshare 14 9 0 0 gcc_trace.txt
./sim gshare 11 5 0 0 jpeg_trace.txt
./sim gshare 10 6 0 0 perl_trace.txt
