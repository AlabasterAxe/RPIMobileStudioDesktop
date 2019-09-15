#!/bin/sh

gcc -Wall -ggdb -I../../rpims_v2 -o spectrum spectrum.c ../../rpims_v2/.libs/librpims.a -lusb -lfftw3 -lGL -lGLU -lglut

