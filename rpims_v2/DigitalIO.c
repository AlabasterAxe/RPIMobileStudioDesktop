/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <string.h>
#include <errno.h>

#include "IOBoard.h"
#include "Features.h"
#include "DigitalIO.h"


int
DigitalIO_SetOpenedDevice(struct DigitalIO *dio, struct IOBoard *dev)
{
	struct DigitalIODev *impl;
	struct Features *feat;
	struct DigitalIODevOps *ops;
	int err;

	if (!dev || !dev->feat)
		return -EOPNOTSUPP;

	feat = (struct Features *) dev->feat;
	if (!feat->digitalIODevOps)
		return -EOPNOTSUPP;

	ops = feat->digitalIODevOps;

	impl = (struct DigitalIODev *) malloc(sizeof(struct DigitalIODev));
	if (!impl)
		return -ENOMEM;

	impl->ops = ops;
	dio->impl = impl;

	err = ops->Init(impl);
	if (err)
		return err;

	return ops->SetOpenedDevice(impl, dev);
}

struct DigitalIO *
DigitalIO_New(struct IOBoard *dev)
{
	struct DigitalIO *dio;
	int err;

	dio = (struct DigitalIO *) malloc(sizeof(struct DigitalIO));
	if (!dio)
		return NULL;

	memset(dio, 0, sizeof(struct DigitalIO));

	err = DigitalIO_SetOpenedDevice(dio, dev);
	if (err) {
		free(dio);
		return NULL;
	}

	return dio;
}

int
DigitalIO_Destroy(struct DigitalIO *dio)
{
	free(dio);
	return 0;
}


int DigitalIO_GetDigitalDirections(struct DigitalIO *dio, unsigned int *dirs)
{
	struct DigitalIODev *impl = dio->impl;
	return impl->ops->GetDigitalDirections(impl, dirs);
}

int DigitalIO_SetDigitalDirections(struct DigitalIO *dio, unsigned int dirs)
{
	struct DigitalIODev *impl = dio->impl;
	return impl->ops->SetDigitalDirections(impl, dirs);
}

int DigitalIO_GetDigitalStates(struct DigitalIO *dio, unsigned int *states)
{
	struct DigitalIODev *impl = dio->impl;
	return impl->ops->GetDigitalStates(impl, states);
}

int DigitalIO_SetDigitalStates(struct DigitalIO *dio, unsigned int states)
{
	struct DigitalIODev *impl = dio->impl;
	return impl->ops->SetDigitalStates(impl, states);
}
