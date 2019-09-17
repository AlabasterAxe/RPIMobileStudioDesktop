/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <stdio.h>
#include <unistd.h>

#include "RPIMS.h"

StreamSample samples[4096];
double data[4096];

int main(int argc, char *argv[])
{
	struct IOBoard *iob = NULL;
	struct AnalogStream *as = NULL;
	unsigned int samplesize, channels;
	int idx;
	int err = 0;

	RPIMS_Init();

	iob = IOBoard_Probe(-1);
	if (!iob) {
		fprintf(stderr, "No IOBoard   device found\n");
		goto exit;
	} else {
		printf("IOBoard  device found!\n");
	}

	// as = AnalogStream_New(iob);
	// if (!as) {
	// 	fprintf(stderr, "Failed to create AnalogStream object\n");
	// 	goto exit;
	// }

	// samplesize = 4096;
	// channels = 1;

	// err = AnalogStream_Start(as);
	// if (err < 0) {
	// 	fprintf(stderr, "Failed to start analog stream\n");
	// 	goto exit;
	// }



	// err = AnalogStream_GetSamples(as, samples, samplesize, channels);
	// if (err < 0) {
	// 	fprintf(stderr, "Failed to get samples\n");
	// 	goto exit;
	// }

	// for (idx = 0; idx < 16; idx++) {
	// 	fprintf(stdout, "%04x ", samples[idx]);
	// }
	// fprintf(stdout, "\n");



	// err = AnalogStream_GetData(as, data, samplesize, channels);
	// if (err < 0) {
	// 	fprintf(stderr, "Failed to get data\n");
	// 	goto exit;
	// }

	// for (idx = 0; idx < 16; idx++) {
	// 	fprintf(stdout, "%.4f ", data[idx]);
	// }
	// fprintf(stdout, "\n");


	// err = AnalogStream_Stop(as);
	// if (err < 0) {
	// 	fprintf(stderr, "Failed to stop analog stream\n");
	// 	goto exit;
	// }

	// AnalogStream_Destroy(as);
	IOBoard_Close(iob);

 exit:
	return err;
}

