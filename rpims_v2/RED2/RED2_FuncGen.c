/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include "IOBoard.h"
#include "FuncGen.h"

#define TIMEOUT 1000


int
RED2_FuncGen_Init(struct FuncGenDev *fg)
{
	int idx;

	fg->priv = NULL;

	for (idx = 0; idx < 2; idx++) {
		fg->channels[idx].frequency = 0;
		fg->channels[idx].phase = 0;
		fg->channels[idx].waveform = FUNC_GEN_WAVEFORM_OFF;
		fg->channels[idx].offset = 0;
		fg->channels[idx].pkPk = 0;
	}

	return 0;
}

int
RED2_FuncGen_Cleanup(struct FuncGenDev *fg)
{
  if(fg->priv)
    free(fg->priv);
  return 0;
}



int
RED2_FuncGen_SetOpenedDevice(struct FuncGenDev *fg, struct IOBoard *dev)
{
  fg->priv = (void *) dev;
  return 0;
}

int
RED2_FuncGen_Start(struct FuncGenDev *fg, int genIndex)
{
	struct IOBoard *dev = (struct IOBoard *) fg->priv;
	return IOBoard_USBControlMsg(dev, 0x40, 0x50, 0, 1, NULL, 0, TIMEOUT);
}

int
RED2_FuncGen_Stop(struct FuncGenDev *fg, int genIndex)
{
	struct IOBoard *dev = (struct IOBoard *) fg->priv;
	return IOBoard_USBControlMsg(dev, 0x40, 0x50, 0, 0, NULL, 0, TIMEOUT);
}

int
RED2_FuncGen_SetFrequency(struct FuncGenDev *fg, int genIndex, double value)
{
	fg->channels[genIndex].frequency = value;
	return 0;
}

int
RED2_FuncGen_SetPhase(struct FuncGenDev *fg, int genIndex, double value)
{
	fg->channels[genIndex].phase = value;
	return 0;
}

int
RED2_FuncGen_SetWaveform(struct FuncGenDev *fg, int genIndex, FuncGenWaveform value)
{
	fg->channels[genIndex].waveform = value;
	return 0;
}

int
RED2_FuncGen_SetOffset(struct FuncGenDev *fg, int genIndex, double value)
{
	fg->channels[genIndex].offset = value;
	return 0;
}

int
RED2_FuncGen_SetPkPk(struct FuncGenDev *fg, int genIndex, double value)
{
	fg->channels[genIndex].pkPk = value;
	return 0;
}


struct FuncGenDevOps RED2_FuncGenDevOps = {
  .Init = RED2_FuncGen_Init,
  .Cleanup = RED2_FuncGen_Cleanup,
  .SetOpenedDevice = RED2_FuncGen_SetOpenedDevice,
  .Start = RED2_FuncGen_Start,
  .Stop = RED2_FuncGen_Stop,
  .SetFrequency = RED2_FuncGen_SetFrequency,
  .SetPhase = RED2_FuncGen_SetPhase,
  .SetWaveform = RED2_FuncGen_SetWaveform,
  .SetOffset = RED2_FuncGen_SetOffset,
  .SetPkPk = RED2_FuncGen_SetPkPk,
};
