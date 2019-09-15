/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <stdlib.h>
#include <errno.h>

#include "IOBoard.h"
#include "DigitalIO.h"


struct RED2_Priv {
	struct IOBoard *dev;
};



int
RED2_DigitalIO_Init(struct DigitalIODev *impl)
{
	struct RED2_Priv *priv;

	priv = malloc(sizeof(struct RED2_Priv));
	if (!priv)
		return -ENOMEM;
	impl->priv = priv;
	return 0;
}

int
RED2_DigitalIO_Cleanup(struct DigitalIODev *impl)
{
	if (impl->priv)
		free(impl->priv);
	return 0;
}

int
RED2_DigitalIO_SetOpenedDevice(struct DigitalIODev *impl, struct IOBoard *dev)
{
	struct RED2_Priv *priv = impl->priv;
	priv->dev = dev;
	return 0;
}


int
RED2_DigitalIO_GetDigitalDirections(struct DigitalIODev *impl, unsigned int *dirs)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
        char buf[2];
	int i;
	int rc;

	rc = IOBoard_USBControlMsg(dev, 0xC3, 0x0040, 0, 0x0000,
				   buf, 2, 1000);
	if (rc < 0)
		return rc;

	*dirs = 0;
	for (i = 0; i < 8; i++)
		*dirs |= ((((unsigned char) buf[1]) >> (7 - i)) & 0x0001) << i;
	for (i = 0; i < 8; i++)
		*dirs |= ((((unsigned char) buf[0]) >> (7 - i)) & 0x0001) << (i + 8);

	//*dirs = (((unsigned char) buf[1]) << 8) | ((unsigned char) buf[0]);

	return 0;
}

int
RED2_DigitalIO_SetDigitalDirections(struct DigitalIODev *impl, unsigned int dirs)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
	unsigned int raw = 0;
	int i;

	raw = 0;
	for (i = 0; i < 16; i++) {
		raw |= ((dirs >> (15 - i)) & 0x0001) << i;
	}

	return IOBoard_USBControlMsg(dev, 0x43, 0x0040, raw, 0x0001,
				     NULL, 0, 1000);
}

int
RED2_DigitalIO_GetDigitalStates(struct DigitalIODev *impl, unsigned int *states)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
	char buf[2];
	int i;
	int rc;

	rc = IOBoard_USBControlMsg(dev, 0xC3, 0x0040, 0, 0x0002,
				   buf, 2, 1000);
	if (rc < 0)
		return rc;

	*states = 0;
	for (i = 0; i < 8; i++)
		*states |= ((((unsigned char) buf[1]) >> (7 - i)) & 0x0001) << i;
	for (i = 0; i < 8; i++)
		*states |= ((((unsigned char) buf[0]) >> (7 - i)) & 0x0001) << (i + 8);

	//*states = (((unsigned char) buf[1]) << 8) | ((unsigned char) buf[0]);

	return 0;
}

int
RED2_DigitalIO_SetDigitalStates(struct DigitalIODev *impl, unsigned int states)
{
	struct RED2_Priv *priv = impl->priv;
	struct IOBoard *dev = priv->dev;
	unsigned int raw;
	int i;

	raw = 0;
	for (i = 0; i < 16; i++) {
		raw |= ((states >> (15 - i)) & 0x0001) << i;
	}

	return IOBoard_USBControlMsg(dev, 0x43, 0x0040, raw, 0x0003,
				     NULL, 0, 1000);
}


struct DigitalIODevOps RED2_DigitalIODevOps = {
	.Init = RED2_DigitalIO_Init,
	.Cleanup = RED2_DigitalIO_Cleanup,
	.SetOpenedDevice = RED2_DigitalIO_SetOpenedDevice,
	.GetDigitalDirections = RED2_DigitalIO_GetDigitalDirections,
	.SetDigitalDirections = RED2_DigitalIO_SetDigitalDirections,
	.GetDigitalStates = RED2_DigitalIO_GetDigitalStates,
	.SetDigitalStates = RED2_DigitalIO_SetDigitalStates,
};

