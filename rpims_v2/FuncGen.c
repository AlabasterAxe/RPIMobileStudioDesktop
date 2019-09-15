/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <errno.h>
#include "FuncGen.h"
#include "IOBoard.h"
#include "Features.h"

int FuncGen_SetOpenedDevice(struct FuncGen *fg, struct IOBoard *dev){
  struct FuncGenDev *impl;
  struct Features *feat;
  struct FuncGenDevOps *ops;
  int err;
  if (!dev || !dev->feat)
    return -EOPNOTSUPP;
  feat = (struct Features *) dev->feat;
  if(!feat->funcGenDevOps)
    return -EOPNOTSUPP;
  ops = feat->funcGenDevOps;
  impl = (struct FuncGenDev *) malloc(sizeof(struct FuncGenDev));

  if(!impl)
    return -ENOMEM;

  impl->ops = ops;
  fg->impl = impl;
  err = ops->Init(impl);
  if(err)
    return err;

  return ops->SetOpenedDevice(impl, dev);
}

struct FuncGen *FuncGen_New(struct IOBoard *dev){
  struct FuncGen *fg;
  int err;

  fg = (struct FuncGen *) malloc(sizeof(struct FuncGen));
  if(!fg)
    return NULL;
  memset(fg, 0, sizeof(struct FuncGen));

  err = FuncGen_SetOpenedDevice(fg, dev);
  if(err){
    free(fg);
    return NULL;
  }
  return  fg;
}

int FuncGen_Destroy(struct FuncGen *fg){
  free(fg);
  return 0;
}

int FuncGen_Start(struct FuncGen *fg, int genIndex){
  struct FuncGenDev *impl = fg->impl;
  return impl->ops->Start(impl, genIndex);
}

int FuncGen_Stop(struct FuncGen *fg, int genIndex){
  struct FuncGenDev *impl = fg->impl;
  return impl->ops->Stop(impl, genIndex);
}

int FuncGen_SetFrequency(struct FuncGen *fg, int genIndex, double value){
  struct FuncGenDev *impl = fg->impl;
  return impl->ops->SetFrequency(impl, genIndex, value);
}

int FuncGen_SetPhase(struct FuncGen *fg, int genIndex, double value){
  struct FuncGenDev *impl = fg->impl;
  return impl->ops->SetPhase(impl, genIndex, value);
}

int FuncGen_SetWaveform(struct FuncGen *fg, int genIndex, FuncGenWaveform value){
  struct FuncGenDev *impl = fg->impl;
  return impl->ops->SetWaveform(impl, genIndex, value);
}

int FuncGen_SetOffset(struct FuncGen *fg, int genIndex, double value){
  struct FuncGenDev *impl = fg->impl;
  return impl->ops->SetOffset(impl, genIndex, value);
}

int FuncGen_SetPkPk(struct FuncGen *fg, int genIndex, double value){
  struct FuncGenDev *impl = fg->impl;
  return impl->ops->SetOffset(impl, genIndex, value);
}

