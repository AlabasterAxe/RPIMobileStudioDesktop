/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <stdlib.h>
#include <errno.h>

#include "IOBoard.h"
#include "DigitalStream.h"

#define SAMPLES_MAX 262144

struct RED2_Priv{
  struct IOBoard *dev;
};

int RED2_DigitalStream_Init(struct DigitalStreamDev *impl){
  struct RED2_Priv *priv;
  priv = malloc(sizeof(struct RED2_Priv));
  if(!priv)
    return -ENOMEM;
  impl->priv = priv;

  return 0;
}

int RED2_DigitalStream_Cleanup(struct DigitalStreamDev *impl){
  if(impl->priv)
    free(impl->priv);
  return 0;
}

int RED2_DigitalStream_SetOpenedDevice(struct DigitalStreamDev *impl, struct IOBoard *dev){
  struct RED2_Priv *priv = impl->priv;
  priv->dev = dev;
  return 0;
}

int RED2_DigitalStream_Start(struct DigitalStreamDev *impl){
  struct RED2_Priv *priv = impl->priv;
  struct IOBoard *dev = priv->dev;

  /*Start Stream*/
  IOBoard_USBControlMsg(dev,0x43,0x0041,0x0001,0x0000, NULL, 0, 1000);
  return 0;
}

int RED2_DigitalStream_Stop(struct DigitalStreamDev *impl){
  struct RED2_Priv *priv = impl->priv;
  struct IOBoard *dev = priv->dev;

  IOBoard_USBControlMsg(dev, 0x43, 0x0041, 0, 0x000, NULL, 0, 1000);
  return 0;
}

int RED2_DigitalStream_GetSamples(struct DigitalStreamDev *impl, StreamSample samples[], unsigned int count){
  struct RED2_Priv *priv = impl->priv;
  struct IOBoard *dev = priv->dev;
  unsigned char tx_buf[4];
  unsigned char rx_buf[512];
  int len;
  int i, idx;
  int rc;
  int start;
  
   if(count > SAMPLES_MAX)
    return -ERANGE;
 
  len = (count/256 + 1) * 256;
  
  tx_buf[0] = len & 0xFF;
  tx_buf[1] = (len >> 8) & 0xFF;
  tx_buf[2] = (len >> 16) & 0xFF;
  tx_buf[3] = (len >> 24) & 0xFF;
  
  rc = IOBoard_USBBulkWrite(dev, 0x02, (char *) tx_buf, 8, 1);
  idx = 0;

  rc = IOBoard_USBBulkRead(dev, 0x01, (char *) rx_buf, 512, 5000);
  if(rc < 0)
    return rc;

   start = 4;//ToDo: remove hardcoding.
    
  for(i = start; i < 8; i +=4){//<--- 8????
     if (idx >= count)
       break;
     samples[idx] = (StreamSample) (((rx_buf[i+1] &0x0F) <<8) | rx_buf[i]);
     idx++;
   }
   return idx;

}

int RED2_DigitalStream_GetData(struct DigitalStreamDev *impl, unsigned char data[], unsigned int count){
  //struct RED2_Priv *priv = impl->priv;
  StreamSample *samples = NULL;
  int idx;
  int rc = 0;

  if(count > SAMPLES_MAX)
    return -ERANGE;
  
  samples = malloc(count * sizeof(StreamSample));
  if(!samples)
    return -ENOMEM;
  
  rc = RED2_DigitalStream_GetSamples(impl, samples, count);
  
  for(idx = 0; idx < rc; idx++){
    data[idx] = ((double)(int)samples[idx]);
    //TODO Offeset correction and gain factor?
  }

  if(samples)
    free(samples);
  return rc;
}

struct DigitalStreamDevOps RED2_DigitalStreamDevOps = {
  .Init = RED2_DigitalStream_Init,
  .Cleanup = RED2_DigitalStream_Cleanup,
  .SetOpenedDevice = RED2_DigitalStream_SetOpenedDevice,
  
  .Start = RED2_DigitalStream_Start,
  .Stop = RED2_DigitalStream_Stop,
  .GetSamples = RED2_DigitalStream_GetSamples,
  .GetData = RED2_DigitalStream_GetData,
};
