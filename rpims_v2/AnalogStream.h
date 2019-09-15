/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#ifndef _ANALOGSTREAM_H
#define _ANALOGSTREAM_H

#include <stdlib.h>
#include <stdint.h>

#include "IOBoard.h"

//typedef int32_t StreamSample;


#define STREAM_CHANNEL_A (1 << 0)
#define STREAM_CHANNEL_B (1 << 1)
#define STREAM_CHANNEL_C (1 << 2)
#define STREAM_CHANNEL_D (1 << 3)


typedef enum {
	STREAM_TRIGGER_MODE_OFF = 0,
	STREAM_TRIGGER_MODE_NORMAL = 1,
	STREAM_TRIGGER_MODE_AUTO = 2
} StreamTriggerMode;

typedef enum {
	STREAM_TRIGGER_SLOPE_RISING = 0,
	STREAM_TRIGGER_SLOPE_FALLING = 1
} StreamTriggerSlope;

typedef enum {
	STREAM_TRIGGER_SOURCE_INTERNAL = 0,
	STREAM_TRIGGER_SOURCE_EXTERNAL = 1
} StreamTriggerSource;

typedef enum {
	STREAM_COLLECT_MODE_BUFFERED = 0,
	STREAM_COLLECT_MODE_STREAMING = 1
} StreamCollectMode;

typedef enum {
	STREAM_CHANNEL_COUPLING_GROUND = 0,
	STREAM_CHANNEL_COUPLING_AC = 1,
	STREAM_CHANNEL_COUPLING_DC = 2
} StreamChannelCoupling;


struct AnalogStreamDev;

struct AnalogStreamDevOps {
	int (*Init)(struct AnalogStreamDev *impl);
	int (*Cleanup)(struct AnalogStreamDev *impl);
	int (*SetOpenedDevice)(struct AnalogStreamDev *impl, struct IOBoard *dev);
	int (*SetChannelGain)(struct AnalogStreamDev *impl, unsigned int channel, unsigned int gain);
	int (*SetTriggerMode)(struct AnalogStreamDev *impl, StreamTriggerMode mode);
	int (*SetTriggerSlope)(struct AnalogStreamDev *impl, StreamTriggerSlope slope);
	int (*SetTriggerSource)(struct AnalogStreamDev *impl, StreamTriggerSource source);
	int (*SetTriggerChannel)(struct AnalogStreamDev *impl, unsigned int channel);
	int (*SetTriggerLevel)(struct AnalogStreamDev *impl, double level);
	int (*Start)(struct AnalogStreamDev *impl);
	int (*Stop)(struct AnalogStreamDev *impl);
	int (*GetSamples)(struct AnalogStreamDev *impl,
			  StreamSample samples[], unsigned int count, unsigned int channels);
	int (*GetData)(struct AnalogStreamDev *impl,
		       double data[], unsigned int count, unsigned int channels);
	int (*GetAvailableSampleRates)(struct AnalogStreamDev *impl, double rates[], unsigned int size);
	int (*SetSampleRateIndex)(struct AnalogStreamDev *impl, unsigned int index);
	int (*GetSampleRateIndex)(struct AnalogStreamDev *impl);
};

struct AnalogStreamDev {
	StreamTriggerMode	triggerMode;
	StreamTriggerSlope	triggerSlope;
	StreamTriggerSource	triggerSource;
	double			triggerLevel;
	unsigned int		triggerChannel;


	StreamCollectMode collectionMode;

	unsigned int pointsPerChannel;
	unsigned int channelCount;
	unsigned int channelMask;

	//AnalogStreamTimeRange timeRange;
	unsigned int avgRuns;
	int running;
	//StreamStatus status = StreamStatus.NoData;
	int throwOutData;
	unsigned int samplingDiv;
	double samplingFreq;
	double dataRatePeriod;
	double sampleRatePeriod;
	double effectiveBandwidth;
	short decimation;


	double FilteringGainCorrection;  // unused in red2
	double startTimeOffset;
	int skip_sampling_div_usb;

	void *priv;

	struct AnalogStreamDevOps *ops;
};


struct AnalogStream {
	struct IOBoard		*dev;
	struct AnalogStreamDev	*impl;

	StreamTriggerMode	triggerMode;
	StreamTriggerSlope	triggerSlope;
	StreamTriggerSource	triggerSource;
	int			triggerChannel;
	double			triggerLevel;

	int			collectionMode;

	unsigned int		pointsPerChannel;
	unsigned int		channelCount;
	unsigned int		averaging;

	double			dataRatePeriod;
	double			sampleRatePeriod;

	unsigned int		samplingDiv;

	double			effectiveBandwidth;
	int			running;
	int			status;
	double			timeRange[2];
	int			suggestedTimeDiv;

	int			supportsPretriggering;
};

struct AnalogStream *AnalogStream_New(struct IOBoard *dev);
int AnalogStream_Destroy(struct AnalogStream *as);

int AnalogStream_SetOpenedDevice(struct AnalogStream *as, struct IOBoard *dev);

int AnalogStream_SetChannelGain(struct AnalogStream *as, unsigned int channel, unsigned int gain);
int AnalogStream_SetTriggerMode(struct AnalogStream *as, StreamTriggerMode mode);
int AnalogStream_SetTriggerSlope(struct AnalogStream *as, StreamTriggerSlope slope);
int AnalogStream_SetTriggerSource(struct AnalogStream *as, StreamTriggerSource source);
int AnalogStream_SetTriggerChannel(struct AnalogStream *as, unsigned int channel);
int AnalogStream_SetTriggerLevel(struct AnalogStream *as, double level);

int AnalogStream_Start(struct AnalogStream *as);
int AnalogStream_Stop(struct AnalogStream *as);
int AnalogStream_GetSamples(struct AnalogStream *as,
			    StreamSample samples[], unsigned int count, unsigned int channels);
int AnalogStream_GetData(struct AnalogStream *as,
			 double data[], unsigned int count, unsigned int channels);

int AnalogStream_GetAvailableSampleRates(struct AnalogStream *as, double rates[], unsigned int size);
int AnalogStream_SetSampleRateIndex(struct AnalogStream *as, unsigned int index);
int AnalogStream_GetSampleRateIndex(struct AnalogStream *as);

int AnalogStream_SetSampleRate(struct AnalogStream *as, double rate);
double AnalogStream_GetSampleRate(struct AnalogStream *as);

#endif /* _ANALOGSTREAM_H */
