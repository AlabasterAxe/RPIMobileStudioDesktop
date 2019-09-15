/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#ifndef _DIGITALIO_H
#define _DIGITALIO_H

#include <stdlib.h>
#include <stdint.h>

#include "IOBoard.h"

#define DIGITALIO_DIR_IN 0
#define DIGITALIO_DIR_OUT 1

#define DIGITALIO_CHANNEL_A (1 << 0)
#define DIGITALIO_CHANNEL_B (1 << 1)
#define DIGITALIO_CHANNEL_C (1 << 2)
#define DIGITALIO_CHANNEL_D (1 << 3)

struct DigitalIODev;

struct DigitalIODevOps {
	int (*Init)(struct DigitalIODev *impl);
	int (*Cleanup)(struct DigitalIODev *impl);
	int (*SetOpenedDevice)(struct DigitalIODev *impl, struct IOBoard *dev);
	int (*GetDigitalDirections)(struct DigitalIODev *impl, unsigned int *dirs);
	int (*SetDigitalDirections)(struct DigitalIODev *impl, unsigned int dirs);
	int (*GetDigitalStates)(struct DigitalIODev *impl, unsigned int *states);
	int (*SetDigitalStates)(struct DigitalIODev *impl, unsigned int states);
};

struct DigitalIODev {
	void *priv;
	struct DigitalIODevOps *ops;
};

struct DigitalIO {
	struct IOBoard		*dev;
	struct DigitalIODev	*impl;
};

struct DigitalIO *DigitalIO_New(struct IOBoard *dev);
int DigitalIO_Destroy(struct DigitalIO *dio);
int DigitalIO_SetOpenedDevice(struct DigitalIO *dio, struct IOBoard *dev);
int DigitalIO_GetDigitalDirections(struct DigitalIO *dio, unsigned int *dirs);
int DigitalIO_SetDigitalDirections(struct DigitalIO *dio, unsigned int dirs);
int DigitalIO_GetDigitalStates(struct DigitalIO *dio, unsigned int *states);
int DigitalIO_SetDigitalStates(struct DigitalIO *dio, unsigned int states);

#endif /* _DIGITALIO_H */
