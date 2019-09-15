/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#ifndef _DIGITALSTREAM_H
#define _DIGITALSTREAM_H

#include <stdlib.h>
#include <stdint.h>

#include "IOBoard.h"

struct DigitalStreamDev;

struct DigitalStreamDevOps{
  int (*Init)(struct DigitalStreamDev *impl);
  int (*Cleanup)(struct DigitalStreamDev *impl);
  int (*SetOpenedDevice)(struct DigitalStreamDev *impl, struct IOBoard *dev);
  int (*Start)(struct DigitalStreamDev *impl);
  int (*Stop)(struct DigitalStreamDev *impl);
  int (*GetSamples) (struct DigitalStreamDev *impl, StreamSample samples[], unsigned int count);
  int (*GetData)(struct DigitalStreamDev *impl, unsigned char data[], unsigned int count);
};

struct DigitalStreamDev{
  unsigned int numSamples;
  void *priv;
  struct DigitalStreamDevOps *ops;

};

struct DigitalStream{
  struct IOBoard *dev;
  struct DigitalStreamDev *impl;
  
  int running;
  int status;
};

struct DigitalStream *DigitalStream_New(struct IOBoard *dev);
int DigitalStream_Destroy(struct DigitalStream *as);

int DigitalStream_SetOpenedDevice(struct DigitalStream *as, struct IOBoard *dev);

int DigitalStream_Start(struct DigitalStream *as);

int DigitalStream_Stop(struct DigitalStream *as);

int DigitalStream_GetSamples(struct DigitalStream *as, StreamSample samples[], unsigned int count);

int DigitalStream_GetData(struct DigitalStream *as, unsigned char data[], unsigned int count);

#endif /* _DIGITALSTREAM_H */
