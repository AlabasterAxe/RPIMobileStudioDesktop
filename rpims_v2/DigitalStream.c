/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <string.h>
#include <errno.h>

#include "IOBoard.h"
#include "Features.h"
#include "DigitalStream.h"

int DigitalStream_SetOpenedDevice(struct DigitalStream *ds, struct IOBoard *dev)
{
  struct DigitalStreamDev *impl;
  struct Features *feat;
  struct DigitalStreamDevOps *ops;
  int err;
 
  if(!dev || !dev->feat)
    return -EOPNOTSUPP;

  feat = (struct Features *) dev->feat;
  if(!feat->digitalStreamDevOps)
    return -EOPNOTSUPP;

  ops = feat->digitalStreamDevOps;
  impl = (struct DigitalStreamDev *) malloc(sizeof(struct DigitalStreamDev));

  if(!impl)
    return -ENOMEM;

  impl->ops = ops;
  ds->impl = impl;
  
  err = ops->Init(impl);
  if(err){
    return err;
  }

  return ops->SetOpenedDevice(impl, dev);
}

struct DigitalStream *DigitalStream_New(struct IOBoard *dev){
  struct DigitalStream *ds;
  int err;

  ds = (struct DigitalStream *) malloc(sizeof(struct DigitalStream));

  if(!ds)
    return NULL;

  memset(ds, 0, sizeof(struct DigitalStream));

  err = DigitalStream_SetOpenedDevice(ds, dev);
  if(err){

    free(ds);
    return NULL;
  }
  return ds;
}

int DigitalStream_Destroy(struct DigitalStream *ds){
  free(ds);
  return 0;
}

int DigitalStream_Start(struct DigitalStream *ds){
  struct DigitalStreamDev *impl = ds->impl;
  return impl->ops->Start(impl);
}

int DigitalStream_Stop(struct DigitalStream *ds){
  struct DigitalStreamDev *impl = ds->impl;
  return impl->ops->Stop(impl);
}

int DigitalStream_GetSamples(struct DigitalStream *ds, StreamSample samples[], unsigned int count){
  
  struct DigitalStreamDev *impl = ds->impl;
  return impl->ops->GetSamples(impl, samples, count);
}

int DigitalStream_GetData(struct DigitalStream *ds, unsigned char data[], unsigned int count){
  struct DigitalStreamDev *impl = ds->impl;
  return impl->ops->GetData(impl, data, count);
}
