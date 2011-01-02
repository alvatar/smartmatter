#!/bin/sh
g++ test-client.cpp -o test-client -I../common -I../ext -lrt
g++ test-server.cpp -o test-server -I../common -I../ext -lrt
