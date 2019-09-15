/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include "IOBoard.h"
#include "Features.h"


extern struct AnalogStreamDevOps RED2_AnalogStreamDevOps;
extern struct DigitalStreamDevOps RED2_DigitalStreamDevOps;
extern struct FuncGenDevOps RED2_FuncGenDevOps;
extern struct DigitalIODevOps RED2_DigitalIODevOps;

static struct Features RED2_Features = {
	.analogStreamDevOps = &RED2_AnalogStreamDevOps,
	.digitalStreamDevOps = &RED2_DigitalStreamDevOps,
	.funcGenDevOps = &RED2_FuncGenDevOps,
	.digitalIODevOps = &RED2_DigitalIODevOps,
};


static int
RED2_IOBoard_Init(struct IOBoard *iob)
{
	iob->feat = &RED2_Features;
	return 0;
}

static int
RED2_IOBoard_Cleanup(struct IOBoard *iob)
{
	return 0;
}


static int
RED2_IOBoard_Open(struct IOBoard *iob)
{
	return IOBoard_USBOpen(iob);
}

static int
RED2_IOBoard_Close(struct IOBoard *iob)
{
	return IOBoard_USBClose(iob);
}

struct IOBoardOps RED2_IOBoardOps = {
	.Init = RED2_IOBoard_Init,
	.Cleanup = RED2_IOBoard_Cleanup,

	.Open = RED2_IOBoard_Open,
	.Close = RED2_IOBoard_Close,
};
