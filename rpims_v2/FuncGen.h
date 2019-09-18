/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#ifndef _FUNCGEN_H
#define _FUNCGEN_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "IOBoard.h"


typedef enum {
  FUNC_GEN_WAVEFORM_OFF = 0,
  FUNC_GEN_WAVEFORM_SINE = 1,
  FUNC_GEN_WAVEFORM_SQUARE = 2,
  FUNC_GEN_WAVEFORM_TRIANGLE = 3
} FuncGenWaveform;

struct FuncGenDev;

struct FuncGenDevOps {
  int (*Init)(struct FuncGenDev *impl);
  int (*Cleanup)(struct FuncGenDev *impl);
  int (*SetOpenedDevice)(struct FuncGenDev *impl, struct IOBoard *dev);
  int (*Start)(struct FuncGenDev *impl, int genIndex);
  int (*Stop)(struct FuncGenDev *impl, int genIndex);
  int (*SetFrequency)(struct FuncGenDev *fg, int genIndex, double value);
  int (*SetPhase)(struct FuncGenDev *fg, int genIndex, double value);
  int (*SetWaveform)(struct FuncGenDev *fg, int genIndex, FuncGenWaveform value);
  int (*SetOffset)(struct FuncGenDev *fg, int genIndex, double value);
  int (*SetPkPk)(struct FuncGenDev *fg, int genIndex, double value);
};

struct FuncGenChannel {
  double frequency;
  double phase;
  FuncGenWaveform waveform;
  double offset;
  double pkPk;
};

struct FuncGenDev {
  struct FuncGenDevOps *ops;
  void *priv;

  struct FuncGenChannel channels[4];
};

struct FuncGen {
  struct IOBoard    *dev;
  struct FuncGenDev  *impl;
};

struct FuncGen *FuncGen_New(struct IOBoard *dev);
int FuncGen_SetOpenedDevice(struct FuncGen *fg, struct IOBoard *dev);
int FuncGen_SetFrequency(struct FuncGen *fg, int genIndex, double value);
int FuncGen_SetPhase(struct FuncGen *fg, int genIndex, double value);
int FuncGen_SetWaveform(struct FuncGen *fg, int genIndex, FuncGenWaveform value);
int FuncGen_SetOffset(struct FuncGen *fg, int genIndex, double value);
int FuncGen_SetPkPk(struct FuncGen *fg, int genIndex, double value);
double FuncGen_GetFrequencyResolution(struct FuncGen *fg, int genIndex);

int FuncGen_Start(struct FuncGen *fg, int genIndex);
int FuncGen_Stop(struct FuncGen *fg, int genIndex);
int FuncGen_Destroy(struct FuncGen *fg);


#endif /* _FUNCGEN_H */
