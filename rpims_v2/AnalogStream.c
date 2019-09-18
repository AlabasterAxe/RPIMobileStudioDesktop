/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <string.h>
#include <errno.h>

#include "IOBoard.h"
#include "Features.h"
#include "AnalogStream.h"



int
AnalogStream_SetOpenedDevice(struct AnalogStream *as, struct IOBoard *dev)
{
  struct AnalogStreamDev *impl;
  struct Features *feat;
  struct AnalogStreamDevOps *ops;
  int err;

  if (!dev || !dev->feat)
    return -EOPNOTSUPP;

  feat = (struct Features *) dev->feat;
  if (!feat->analogStreamDevOps)
    return -EOPNOTSUPP;

  ops = feat->analogStreamDevOps;

  impl = (struct AnalogStreamDev *) malloc(sizeof(struct AnalogStreamDev));
  if (!impl)
    return -ENOMEM;

  impl->ops = ops;
  as->impl = impl;

  err = ops->Init(impl);
  if (err)
    return err;

  return ops->SetOpenedDevice(impl, dev);
}

struct AnalogStream *
AnalogStream_New(struct IOBoard *dev)
{
  struct AnalogStream *as;
  int err;

  as = (struct AnalogStream *) malloc(sizeof(struct AnalogStream));
  if (!as)
    return NULL;

  memset(as, 0, sizeof(struct AnalogStream));

  err = AnalogStream_SetOpenedDevice(as, dev);
  if (err) {
    free(as);
    return NULL;
  }

  return as;
}

int
AnalogStream_Destroy(struct AnalogStream *as)
{
  free(as);
  return 0;
}

int AnalogStream_SetTriggerMode(struct AnalogStream *as, StreamTriggerMode mode){
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->SetTriggerMode(impl, mode);
}

int AnalogStream_SetTriggerSlope(struct AnalogStream *as, StreamTriggerSlope slope){
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->SetTriggerSlope(impl, slope);
}

int AnalogStream_SetTriggerSource(struct AnalogStream *as, StreamTriggerSource source){
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->SetTriggerSlope(impl, source);
}

int AnalogStream_SetTriggerChannel(struct AnalogStream *as, unsigned int channel){
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->SetTriggerChannel(impl, channel);
}

int AnalogStream_SetTriggerLevel(struct AnalogStream *as, double level){
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->SetTriggerLevel(impl, level);
}

int
AnalogStream_SetChannelGain(struct AnalogStream *as, unsigned int channel, unsigned int gain)
{
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->SetChannelGain(impl, channel, gain);
}

int
AnalogStream_Start(struct AnalogStream *as)
{
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->Start(impl);
}

int
AnalogStream_Stop(struct AnalogStream *as)
{
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->Stop(impl);
}

int
AnalogStream_GetSamples(struct AnalogStream *as,
      StreamSample samples[], unsigned int count, unsigned int channels)
{
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->GetSamples(impl, samples, count, channels);
}

int
AnalogStream_GetData(struct AnalogStream *as,
         double data[], unsigned int count, unsigned int channels)
{
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->GetData(impl, data, count, channels);
}

int
AnalogStream_GetAvailableSampleRates(struct AnalogStream *as, double rates[], unsigned int size)
{
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->GetAvailableSampleRates(impl, rates, size);
}

int
AnalogStream_SetSampleRateIndex(struct AnalogStream *as, unsigned int index)
{
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->SetSampleRateIndex(impl, index);
}

int
AnalogStream_GetSampleRateIndex(struct AnalogStream *as)
{
  struct AnalogStreamDev *impl = as->impl;
  return impl->ops->GetSampleRateIndex(impl);
}

int
AnalogStream_SetSampleRate(struct AnalogStream *as, double rate)
{
  double rates[32];
  int rc;
  int index;
  int i;

  rc = AnalogStream_GetAvailableSampleRates(as, rates, 32);
  if (rc < 0)
    return -EINVAL;

  if (rate <= rates[0]) {
    index = 0;
    goto out;
  }

  for (i = 0; i < (rc - 1); i++) {
    if (rate >= rates[i] && rate <= rates[i + 1]) {
      if ((rate - rates[i]) < (rates[i + 1] - rate))
        index = i;
      else
        index = i + 1;
      goto out;
    }
  }

  index = rc - 1;

 out:
  return AnalogStream_SetSampleRateIndex(as, index);
}

double
AnalogStream_GetSampleRate(struct AnalogStream *as)
{
  double rates[32];
  int rc;
  int index;

  rc = AnalogStream_GetAvailableSampleRates(as, rates, 32);
  if (rc < 0)
    return 0.0;

  index = AnalogStream_GetSampleRateIndex(as);
  if (index < 0 || index >= rc)
    return 0.0;

  return rates[index];
}



