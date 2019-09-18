/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <libusb.h>
#include <stdio.h>

#include "IOBoard.h"

extern struct IOBoardOps RED2_IOBoardOps;

extern struct IOBoard *IOBoard_Probe_libusb(int type);

int
RPIMS_Init(void)
{
  libusb_init(NULL);
  return 0;
}


struct IOBoard *
IOBoard_Probe(int type) {
  struct IOBoard *iob;
  int product_id;
  int err;

  /* Default to using libusb interface. This could change in the future */
  iob = IOBoard_Probe_libusb(type);
  if (!iob) {
    return NULL;
  }

  /* XXX-TODO: Use lookup table for ProductID and IOBoardOps */
  product_id = IOBoard_USBGetProductId(iob);
  printf("Product id: %x\n", product_id);
  if (product_id == 0x1000) {
    //iob->ops = &ARM_IOBoardOps;
    iob->ops = NULL;
  } else if (product_id == 0x2100) {
    iob->ops = &RED2_IOBoardOps;
  } else {
    iob->ops = NULL;
  }

  /* Call IOBoard init function, if it has one */
  if (iob->ops && iob->ops->Init) {
    err = iob->ops->Init(iob);
    if (err) {
      fprintf(stderr, "There was an error initing the device.\n");
      free(iob);
      return NULL;
    }

    err = iob->ops->Open(iob);
    if (err) {
      fprintf(stderr, "There was an error opening the device: %d \n");
      free(iob);
      return NULL;
    }
  }

  return iob;
}

int
IOBoard_Init(struct IOBoard *iob)
{
  return iob->ops->Init(iob);
}

int
IOBoard_Cleanup(struct IOBoard *iob)
{
  return iob->ops->Cleanup(iob);
}


int
IOBoard_Open(struct IOBoard *iob)
{
  return iob->ops->Open(iob);
}

int
IOBoard_Close(struct IOBoard *iob)
{
  return iob->ops->Close(iob);
}



int
IOBoard_USBOpen(struct IOBoard *iob)
{
  return iob->usbops->Open(iob);
}

int
IOBoard_USBClose(struct IOBoard *iob)
{
  return iob->usbops->Close(iob);
}

int
IOBoard_USBControlMsg(struct IOBoard *iob, int type, int req, int value,
          int index, char *data, int size, int timeout)
{
  return iob->usbops->ControlMsg(iob, type, req, value,
               index, data, size, timeout);
}

int
IOBoard_USBBulkRead(struct IOBoard *iob, int endpoint, char *data, int len, int timeout)
{
  return iob->usbops->BulkRead(iob, endpoint, data, len, timeout);
}

int
IOBoard_USBBulkWrite(struct IOBoard *iob, int endpoint, char *data, int len, int timeout)
{
  return iob->usbops->BulkWrite(iob, endpoint, data, len, timeout);
}

int
IOBoard_USBInterruptRead(struct IOBoard *iob, int endpoint, char *data, int len, int timeout)
{
  return iob->usbops->InterruptRead(iob, endpoint, data, len, timeout);
}

int
IOBoard_USBInterruptWrite(struct IOBoard *iob, int endpoint, char *data, int len, int timeout)
{
  return iob->usbops->InterruptWrite(iob, endpoint, data, len, timeout);
}

int
IOBoard_USBReset(struct IOBoard *iob)
{
  return iob->usbops->Reset(iob);
}

int
IOBoard_USBGetProductId(struct IOBoard *iob)
{
  return iob->usbops->GetProductId(iob);
}


