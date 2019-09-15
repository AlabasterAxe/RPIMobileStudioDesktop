/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <stdio.h>

#include "RPIMS.h"

StreamSample samples[4096];
double data[4096];


/*
 * Integer handles for RPIMS objects
 */
static int iob_handle;
static int as_handle;


int
Initialize_Devices(void)
{
	struct IOBoard *iob = NULL;
	struct AnalogStream *as = NULL;

	/*
	 * Initialize Objects
	 *
	 * Here you instantiate the IOBoard and whatever device
	 * components you may need (e.g. AnalogStream)
	 */
	RPIMS_Init();

	iob = IOBoard_Probe(-1);
	if (!iob) {
		fprintf(stderr, "No IOBoard device found\n");
		goto error;
	}

	as = AnalogStream_New(iob);
	if (!as) {
		fprintf(stderr, "Failde to create AnalogStream object\n");
		goto error;
	}

	/*
	 * Add Objects to ObjTable
	 *
	 * Here you add the references to the objects to the table
	 * and get back integer handles for them. We also check for
	 * error conditions here, since this operation may fail
	 * (in the event we run out of table space).
	 */
	iob_handle = ObjTable_Add(iob);
	if (iob_handle < 0) {
		fprintf(stderr, "Failed to add object to table\n");
		goto error;
	}

	as_handle = ObjTable_Add(as);
	if (as_handle < 0) {
		fprintf(stderr, "Failed to add object to table\n");
		goto error;
	}

	return 0;

 error:
	if (as)
		AnalogStream_Destroy(as);
	if (iob)
		IOBoard_Close(iob);
	return -1;
}


int
Use_Devices(void)
{
	struct AnalogStream *as;
	unsigned int samplesize = 4096;
	unsigned int channels = 1;
	int err = 0;
	int idx;


	/*
	 * Get device from ObjTable
	 */
	as = ObjTable_Get(as_handle);
	if (!as) {
		fprintf(stderr, "Invalid handle %d\n", as_handle);
		return -1;
	}



	err = AnalogStream_Start(as);
	if (err < 0) {
		fprintf(stderr, "Failed to start analog stream\n");
		goto stop;
	}

	err = AnalogStream_GetSamples(as, samples, samplesize, channels);
	if (err < 0) {
		fprintf(stderr, "Failed to get samples\n");
		goto stop;
	}

	for (idx = 0; idx < 16; idx++) {
		fprintf(stdout, "%04x ", samples[idx]);
	}
	fprintf(stdout, "\n");

	err = AnalogStream_GetData(as, data, samplesize, channels);
	if (err < 0) {
		fprintf(stderr, "Failed to get data\n");
		goto stop;
	}

	for (idx = 0; idx < 16; idx++) {
		fprintf(stdout, "%.4f ", data[idx]);
	}
	fprintf(stdout, "\n");

 stop:
	err = AnalogStream_Stop(as);
	if (err < 0) {
		fprintf(stderr, "Failed to stop analog stream\n");
	}

	return 0;
}








int main(int argc, char *argv[])
{
	int err;

	err = Initialize_Devices();
	if (err < 0)
		return 1;
	Use_Devices();

	return 0;
}

