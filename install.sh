#! /bin/bash 
g++ -g -Wall -std=c++11 Fp16Convert.c interpret_output.cpp Region.cpp Common.cpp test.cpp -o test -I /usr/local/include -I /usr/local/include/libusb-1.0 -L /usr/local/lib -L ../Deepano_SDK_AND_EXAMPLES `pkg-config opencv --cflags --libs` -lpthread -lusb-1.0 -ldp_api
