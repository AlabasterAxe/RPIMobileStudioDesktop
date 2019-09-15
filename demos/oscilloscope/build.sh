#!/bin/sh

gcc -Wall -ggdb -I../../rpims_v2 -o osc osc.c ../../rpims_v2/.libs/librpims.a -lusb -lGL -lGLU -lglut

