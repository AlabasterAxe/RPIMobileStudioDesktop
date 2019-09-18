#!/bin/sh

gcc -Wall -ggdb -I../../rpims_v2 -o osc osc.c ../../rpims_v2/librpims.a -lpthread -lusb -lusb-1.0 -lGL -lGLU -lglut

