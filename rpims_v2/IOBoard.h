/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#ifndef _IOBOARD_H
#define _IOBOARD_H

#include <stdint.h>

#define RPIMS_VENDOR_ID 0x181c

typedef int32_t StreamSample;

struct IOBoard;

struct IOBoardOps {
  int (*Init)(struct IOBoard *iob);
  int (*Cleanup)(struct IOBoard *iob);

  int (*Open)(struct IOBoard *iob);
  int (*Close)(struct IOBoard *iob);
};

struct IOBoardUSBOps {
  int (*Open)(struct IOBoard *iob);
  int (*Close)(struct IOBoard *iob);
  int (*ControlMsg)(struct IOBoard *iob, int type, int req, int index,
        int value, char *data, int size, int timeout);
  int (*BulkRead)(struct IOBoard *iob, int endpoint, char *data,
      int len, int timeout);
  int (*BulkWrite)(struct IOBoard *iob, int endpoint, char *data,
       int len, int timeout);
  int (*InterruptRead)(struct IOBoard *iob, int endpoint, char *data,
           int len, int timeout);
  int (*InterruptWrite)(struct IOBoard *iob, int endpoint, char *data,
            int len, int timeout);
  int (*Reset)(struct IOBoard *iob);
  int (*GetProductId)(struct IOBoard *iob);
};

struct Features;

struct IOBoard {
  struct IOBoardUSBOps  *usbops;
  void      *usbpriv;

  struct IOBoardOps  *ops;
  struct Features    *feat;

  void *priv;
};

struct IOBoard *IOBoard_Probe(int type);
int IOBoard_Open(struct IOBoard *iob);
int IOBoard_Close(struct IOBoard *iob);
int IOBoard_Cleanup(struct IOBoard *iob);

int IOBoard_USBOpen(struct IOBoard *iob);
int IOBoard_USBClose(struct IOBoard *iob);
int IOBoard_USBControlMsg(struct IOBoard *iob, int type, int req, int index,
        int value, char *data, int size, int timeout);
int IOBoard_USBBulkRead(struct IOBoard *iob, int endpoint, char *data, int len, int timeout);
int IOBoard_USBBulkWrite(struct IOBoard *iob, int endpoint, char *data, int len, int timeout);
int IOBoard_USBInterruptRead(struct IOBoard *iob, int endpoint, char *data, int len, int timeout);
int IOBoard_USBInterruptWrite(struct IOBoard *iob, int endpoint, char *data, int len, int timeout);
int IOBoard_USBReset(struct IOBoard *iob);
int IOBoard_USBGetProductId(struct IOBoard *iob);

#endif /* _IOBOARD_H */
