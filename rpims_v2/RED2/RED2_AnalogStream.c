/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <stdlib.h>
#include <errno.h>

#include "IOBoard.h"
#include "AnalogStream.h"

#include <stdio.h>


#define SAMPLES_MAX 262144
#define MAX_ADC_VOLTAGE 3.3
#define MAX_ADC_RESULT 4096.0

static double RED2_SampleRates[] = {93.0, 186.0, 372.0, 744.0, 1488.0, 2976.0, 5932.0, 11860.0, 23620.0, 46840.0, 92300.0, 179200.0, 338660.0, 609600.0, 1015600.0, 1524000.0};
static unsigned int RED2_SampleRatesSize = 16;


struct RED2_AnalogStreamChannel {
	unsigned int gainMode;
	double gainFactor;
	double offsetCorrection;
	StreamChannelCoupling coupling;
};

struct RED2_Priv {
	struct IOBoard *dev;
	struct RED2_AnalogStreamChannel channels[2];
	int sample_div;
};



int
RED2_AnalogStream_Init(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv;

	priv = malloc(sizeof(struct RED2_Priv));
	if (!priv)
		return -ENOMEM;

	priv->channels[0].gainMode = 0;
	priv->channels[0].gainFactor = 1.0;
	priv->channels[0].offsetCorrection = 0.0;
	priv->channels[0].coupling = STREAM_CHANNEL_COUPLING_DC;

	priv->channels[1].gainMode = 0;
	priv->channels[1].gainFactor = 1.0;
	priv->channels[1].offsetCorrection = 0.0;
	priv->channels[1].coupling = STREAM_CHANNEL_COUPLING_DC;

	priv->sample_div = 1;

	impl->priv = priv;

	impl->triggerMode = STREAM_TRIGGER_MODE_OFF;
	impl->triggerSlope = STREAM_TRIGGER_SLOPE_RISING;
	impl->triggerSource = STREAM_TRIGGER_SOURCE_INTERNAL;
	impl->triggerLevel = 0.0;
	impl->triggerChannel = STREAM_CHANNEL_A;

	return 0;
}

int
RED2_AnalogStream_Cleanup(struct AnalogStreamDev *impl)
{
	if (impl->priv)
		free(impl->priv);
	return 0;
}

int
RED2_AnalogStream_SetOpenedDevice(struct AnalogStreamDev *impl, struct IOBoard *dev)
{
	struct RED2_Priv *priv = impl->priv;
	priv->dev = dev;
	return 0;
}

int
RED2_AnalogStream_SetTriggerLevel_SET(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
	int trig_ch;
	double level;
	short internal_level;

	if (impl->triggerChannel == STREAM_CHANNEL_A)
		trig_ch = 0;
	else if (impl->triggerChannel == STREAM_CHANNEL_B)
		trig_ch = 1;
	else
		return -EINVAL;


	level = impl->triggerLevel;
	level /= priv->channels[trig_ch].gainFactor;
	level += priv->channels[trig_ch].offsetCorrection;
	level += 2.5;

	internal_level = (short)(level * MAX_ADC_RESULT / MAX_ADC_VOLTAGE);

	/* Trigger Level */
	return IOBoard_USBControlMsg(dev, 0x43, 0x0023, internal_level, 0x0003,
				     NULL, 0, 1000);
}

int
RED2_AnalogStream_SetTriggerMode_SET(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
	int value;

	if (impl->triggerMode == STREAM_TRIGGER_MODE_OFF)
		value = 2;
	else if (impl->triggerMode == STREAM_TRIGGER_MODE_NORMAL)
		value = 1;
	else if (impl->triggerMode == STREAM_TRIGGER_MODE_AUTO)
		value = 0;
	else return -EINVAL;

	/* Trigger (0 = Auto, 1 = Normal, 2 = Off) */
	return IOBoard_USBControlMsg(dev, 0x43, 0x0023, value, 0x0001,
				     NULL, 0, 1000);
}

int
RED2_AnalogStream_SetTriggerSlope_SET(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
	int value;

	if (impl->triggerSlope == STREAM_TRIGGER_SLOPE_RISING)
		value = 1;
	else if (impl->triggerSlope == STREAM_TRIGGER_SLOPE_FALLING)
		value = 0;
	else
		return -EINVAL;

	/* Trigger Slope (0 = Falling, 1 = Rising) */
	return IOBoard_USBControlMsg(dev, 0x43, 0x0023, value, 0x0002,
				     NULL, 0, 1000);
}

int
RED2_AnalogStream_SetTriggerSource_SET(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
	int value;

	if (impl->triggerSource == STREAM_TRIGGER_SOURCE_INTERNAL)
		value = 0;
	else if (impl->triggerSource == STREAM_TRIGGER_SOURCE_EXTERNAL)
		value = 1;
	else
		return -EINVAL;

	/* Trigger Source (0 = Internal, 1 = External) */
	return IOBoard_USBControlMsg(dev, 0x43, 0x0023, value, 0x0004,
				     NULL, 0, 1000);
}

int
RED2_AnalogStream_SetTriggerChannel_SET(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
	int value;

	if (impl->triggerChannel == STREAM_CHANNEL_A)
		value = 0;
	else if (impl->triggerChannel == STREAM_CHANNEL_B)
		value = 1;
	else
		return -EINVAL;

	/* Trigger Channel */
	return IOBoard_USBControlMsg(dev, 0x43, 0x0023, value, 0x0000,
				     NULL, 0, 1000);
}



int
RED2_AnalogStream_SetTriggerLevel(struct AnalogStreamDev *impl, double level)
{
	impl->triggerLevel = level;
	return RED2_AnalogStream_SetTriggerLevel_SET(impl);
}

int
RED2_AnalogStream_SetTriggerMode(struct AnalogStreamDev *impl, StreamTriggerMode mode)
{
	impl->triggerMode = mode;
	return RED2_AnalogStream_SetTriggerMode_SET(impl);
}

int
RED2_AnalogStream_SetTriggerSlope(struct AnalogStreamDev *impl, StreamTriggerSlope slope)
{
	impl->triggerSlope = slope;
	return RED2_AnalogStream_SetTriggerSlope_SET(impl);
}

int
RED2_AnalogStream_SetTriggerSource(struct AnalogStreamDev *impl, StreamTriggerSource source)
{
	impl->triggerSource = source;
	return RED2_AnalogStream_SetTriggerSource_SET(impl);
}

int
RED2_AnalogStream_SetTriggerChannel(struct AnalogStreamDev *impl, unsigned int channel)
{
	impl->triggerChannel = channel;
	return RED2_AnalogStream_SetTriggerChannel_SET(impl);
}





int
RED2_AnalogStream_SetSampleRate_SET(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;

	/*
	 * Sample Rates:
	 *
	 * 1.0150 MSPS: SAMPLE = 2, PLL = 3
	 * 1.5225 MSPS: SAMPLE = 1, PLL = 4
	 */

	/* Sampling DIV */
	IOBoard_USBControlMsg(dev, 0x43, 0x0024, priv->sample_div, 0x0000,
			      NULL, 0, 1000);

	/* PLL DIV */
	IOBoard_USBControlMsg(dev, 0x43, 0x0025, 4, 0x0000,
			      NULL, 0, 1000);

	/* Decimation */
	IOBoard_USBControlMsg(dev, 0x43, 0x0028, 1, 0x0000,
			      NULL, 0, 1000);

	return 0;
}


int
RED2_AnalogStream_SetChannelGain_SET(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
	unsigned short a1, a2, b1, b2;

	switch (priv->channels[0].gainMode) {
	case 1:
		/* 0.75 V/div */
		a1 = 0; a2 = 1;
		break;
	case 2:
		/* 162.5 mV/div */
		a1 = 0; a2 = 0;
		break;
	case 3:
		/* 87.5 mV/div */
		a1 = 1; a2 = 0;
		break;
	case 4:
		/* 8.75 mV/div */
		a1 = 2; a2 = 0;
		break;
	case 5:
		/* 0.875 mV/div */
		a1 = 3; a2 = 0;
		break;
	case 6:
		/* < 0.875 mV/div */
		a1 = 4; a2 = 0;
		break;
	case 0:
	default:
		/* 4 V/div */
		a1 = 0; a2 = 2;
		break;
	}

	switch (priv->channels[1].gainMode) {
	case 1:
		/* 0.75 V/div */
		b1 = 0; b2 = 1;
		break;
	case 2:
		/* 162.5 mV/div */
		b1 = 0; b2 = 0;
		break;
	case 3:
		/* 87.5 mV/div */
		b1 = 1; b2 = 0;
		break;
	case 4:
		/* 8.75 mV/div */
		b1 = 2; b2 = 0;
		break;
	case 5:
		/* 0.875 mV/div */
		b1 = 3; b2 = 0;
		break;
	case 6:
		/* < 0.875 mV/div */
		b1 = 4; b2 = 0;
		break;
	case 0:
	default:
		/* 4 V/div */
		b1 = 0; b2 = 2;
		break;
	}

	/* Set Channel A Gain */
	IOBoard_USBControlMsg(dev, 0x43, 0x0022, a1, 0,
			      NULL, 0, 1000);
	IOBoard_USBControlMsg(dev, 0x43, 0x0022, a2, (0x0010 | 0),
			      NULL, 0, 1000);
	IOBoard_USBControlMsg(dev, 0x43, 0x0022, 0, (0x0020 | 0),
			      NULL, 0, 1000);

	/* Set Channel B Gain */
	IOBoard_USBControlMsg(dev, 0x43, 0x0022, b1, 1,
			      NULL, 0, 1000);
	IOBoard_USBControlMsg(dev, 0x43, 0x0022, b2, (0x0010 | 1),
			      NULL, 0, 1000);
	IOBoard_USBControlMsg(dev, 0x43, 0x0022, 0, (0x0020 | 1),
			      NULL, 0, 1000);

	return 0;
}

int
RED2_AnalogStream_SetChannelGain(struct AnalogStreamDev *impl, unsigned int channel, unsigned int gain)
{
	struct RED2_Priv *priv = impl->priv;
	unsigned int ch = channel % 2;

	priv->channels[ch].gainMode = gain;
	switch (gain) {
	case 1:
		priv->channels[ch].gainFactor = 214.093;
		break;
	case 2:
		priv->channels[ch].gainFactor = 1200.0;
		break;
	case 3:
		priv->channels[ch].gainFactor = 5480.0;
		break;
	case 4:
		priv->channels[ch].gainFactor = 10564.885;
		break;
	case 5:
		priv->channels[ch].gainFactor = 72400.0;
		break;
	case 6:
		priv->channels[ch].gainFactor = 185636.36;
		break;
	case 0:
	default:
		priv->channels[ch].gainFactor = 39.166666;
		break;
 
	}
	RED2_AnalogStream_SetChannelGain_SET(impl);
	return 0;
}


int
RED2_AnalogStream_Start(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;

	RED2_AnalogStream_SetSampleRate_SET(impl);

	RED2_AnalogStream_SetChannelGain(impl, 0, priv->channels[0].gainMode);
	RED2_AnalogStream_SetChannelGain(impl, 1, priv->channels[1].gainMode);

	RED2_AnalogStream_SetTriggerLevel_SET(impl);
	RED2_AnalogStream_SetTriggerMode_SET(impl);
	RED2_AnalogStream_SetTriggerSlope_SET(impl);
	RED2_AnalogStream_SetTriggerSource_SET(impl);
	RED2_AnalogStream_SetTriggerChannel_SET(impl);

	/* Start Stream */
        IOBoard_USBControlMsg(dev, 0x43, 0x0020, 1, 0x0000,
			      NULL, 0, 1000);
	return 0;
}

int
RED2_AnalogStream_Stop(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;

	IOBoard_USBControlMsg(dev, 0x43, 0x0020, 0, 0x0000, NULL, 0, 1000);
	return 0;
}


int
RED2_AnalogStream_GetSamples(struct AnalogStreamDev *impl,
			     StreamSample samples[], unsigned int count, unsigned int channels)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
	unsigned char tx_buf[8];
	unsigned char rx_buf[512];
	unsigned int nch;
	int shorts;
	int size;
	int offset = 0;
	int bytes_read = 0;
	int i, idx;
	int rc;
	int start;
	int status;

	if (count > SAMPLES_MAX) {
		return -ERANGE;
	}

	nch = !!(channels & STREAM_CHANNEL_A) + !!(channels & STREAM_CHANNEL_B);
	if (nch == 2)
		shorts = (count / 2) * 2 + 2;
	else if (nch == 1)
		shorts = count * 2 + 2;
	else
		return 0;


	shorts += 512 - (shorts % 512);
	size = shorts * 2;

	tx_buf[0] = shorts & 0xFF;
	tx_buf[1] = (shorts >> 8) & 0xFF;
	tx_buf[2] = (shorts >> 16) & 0xFF;
	tx_buf[3] = (shorts >> 24) & 0xFF;

	tx_buf[4] = offset & 0xFF;
	tx_buf[5] = (offset >> 8) & 0xFF;
	tx_buf[6] = (offset >> 16) & 0xFF;
	tx_buf[7] = (offset >> 24) & 0xFF;


	rc = IOBoard_USBBulkWrite(dev, 0x02, (char *) tx_buf, 8, 1);
	fprintf(stdout, "Write: %d\n", rc);

	idx = 0;
	while (bytes_read < size) {

		rc = IOBoard_USBBulkRead(dev, 0x01, (char *) rx_buf, 512, 60000);
		fprintf(stdout, "Read: %d\n", rc);

		if (rc == 0)
			continue;
		if (rc < 0)
			return rc;

		bytes_read += rc;

		if (bytes_read <= 512) {
			status = rx_buf[1] << 8 | rx_buf[0];
			start = 4;
		} else {
			start = 0;
		}

		for (i = start; i < 512; i += 4) {
			if (idx >= count)
				break;

			/* Channel A */
			if (channels & STREAM_CHANNEL_A) {
				samples[idx] = (StreamSample)
					(((rx_buf[i+1] & 0x0F) << 8) | rx_buf[i]);
				samples[idx] = (samples[idx] % 2048) + -2048 * (samples[idx] / 2048);
				idx++;
			}

			if (idx >= count)
				break;

			/* Channel B */
			if (channels & STREAM_CHANNEL_B) {
				samples[idx] = (StreamSample)
					(((rx_buf[i+3] & 0x0F) << 8) | rx_buf[i+2]);
				samples[idx] = (samples[idx] % 2048) + -2048 * (samples[idx] / 2048);
				idx++;
			}
		}
	}

	return idx;
}


int
RED2_AnalogStream_GetData(struct AnalogStreamDev *impl,
			  double data[], unsigned int count, unsigned int channels)
{
	struct RED2_Priv *priv = impl->priv;
	StreamSample *samples = NULL;
	int idx;
	unsigned int nch;
	int rc = 0;

	if (count > SAMPLES_MAX)
		return -ERANGE;

	samples = malloc(count * sizeof(StreamSample));
	if (!samples)
		return -ENOMEM;

	nch = !!(channels & STREAM_CHANNEL_A) + !!(channels & STREAM_CHANNEL_B);

	rc = RED2_AnalogStream_GetSamples(impl, samples, count, channels);
	if (rc < 0)
		goto out;

	for (idx = 0; idx < rc; idx++) {
		int ch = idx % nch;

		data[idx] = (double)(int)(((samples[idx] + 0x0800) % 0x1000) - 0x0800);
		data[idx] /= priv->channels[ch].gainFactor;
		//data[idx] *= MAX_ADC_VOLTAGE / MAX_ADC_RESULT;
		//data[idx] -= 2.5;
		//data[idx] -= priv->channels[ch].offsetCorrection;
		//data[idx] *= priv->channels[ch].gainFactor;
	}

 out:
	if (samples)
		free(samples);
	return rc;
}

int
RED2_AnalogStream_GetAvailableSampleRates(struct AnalogStreamDev *impl, double rates[], unsigned int size)
{
	int i;

	if (size < RED2_SampleRatesSize)
		return -EOVERFLOW;

	for (i = 0; i < RED2_SampleRatesSize; i++)
		rates[i] = RED2_SampleRates[i];

	return RED2_SampleRatesSize;
}

int
RED2_AnalogStream_SetSampleRateIndex(struct AnalogStreamDev *impl, unsigned int index)
{
	struct RED2_Priv *priv = impl->priv;

	switch (index) {
	case 0:
		priv->sample_div = 32768;
		break;
	case 1:
		priv->sample_div = 16384;
		break;
	case 2:
		priv->sample_div = 8192;
		break;
	case 3:
		priv->sample_div = 4096;
		break;
	case 4:
		priv->sample_div = 2048;
		break;
	case 5:
		priv->sample_div = 1024;
		break;
	case 6:
		priv->sample_div = 512;
		break;
	case 7:
		priv->sample_div = 256;
		break;
	case 8:
		priv->sample_div = 128;
		break;
	case 9:
		priv->sample_div = 64;
		break;
	case 10:
		priv->sample_div = 32;
		break;
	case 11:
		priv->sample_div = 16;
		break;
	case 12:
		priv->sample_div = 8;
		break;
	case 13:
		priv->sample_div = 4;
		break;
	case 14:
		priv->sample_div = 2;
		break;
	case 15:
		priv->sample_div = 1;
		break;
	default:
		return -EINVAL;
	}

	RED2_AnalogStream_SetSampleRate_SET(impl);
	return 0;
}

int
RED2_AnalogStream_GetSampleRateIndex(struct AnalogStreamDev *impl)
{
	struct RED2_Priv *priv = impl->priv;

	switch(priv->sample_div) {
	case 32768:
		return 0;
	case 16384:
		return 1;
	case 8192:
		return 2;
	case 4096:
		return 3;
	case 2048:
		return 4;
	case 1024:
		return 5;
	case 512:
		return 6;
	case 256:
		return 7;
	case 128:
		return 8;
	case 64:
		return 9;
	case 32:
		return 10;
	case 16:
		return 11;
	case 8:
		return 12;
	case 4:
		return 13;
	case 2:
		return 14;
	case 1:
		return 15;
	}
	return -EINVAL;
}


struct AnalogStreamDevOps RED2_AnalogStreamDevOps = {
	.Init = RED2_AnalogStream_Init,
	.Cleanup = RED2_AnalogStream_Cleanup,
	.SetOpenedDevice = RED2_AnalogStream_SetOpenedDevice,
	.Start = RED2_AnalogStream_Start,
	.Stop = RED2_AnalogStream_Stop,
	.GetSamples = RED2_AnalogStream_GetSamples,
	.GetData = RED2_AnalogStream_GetData,
	.SetChannelGain = RED2_AnalogStream_SetChannelGain,
	.SetTriggerMode = RED2_AnalogStream_SetTriggerMode,
	.SetTriggerSlope = RED2_AnalogStream_SetTriggerSlope,
	.SetTriggerSource = RED2_AnalogStream_SetTriggerSource,
	.SetTriggerChannel = RED2_AnalogStream_SetTriggerChannel,
	.SetTriggerLevel = RED2_AnalogStream_SetTriggerLevel,
	.GetAvailableSampleRates = RED2_AnalogStream_GetAvailableSampleRates,
	.SetSampleRateIndex = RED2_AnalogStream_SetSampleRateIndex,
	.GetSampleRateIndex = RED2_AnalogStream_GetSampleRateIndex,
};

