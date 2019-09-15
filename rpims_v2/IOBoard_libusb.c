/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <usb.h>

#include "IOBoard.h"

struct IOBoardUSBPriv_libusb {
	struct usb_device	*dev;
	struct usb_dev_handle	*hnd;
};

static int
IOBoard_USBOps_Open_libusb(struct IOBoard *iob)
{
	struct IOBoardUSBPriv_libusb *priv;
	struct usb_dev_handle *hnd;
	int err;

	priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;

	/* Reset USB device */
	hnd = usb_open(priv->dev);
	if (!hnd)
		return -1;

	err = usb_reset(hnd);
	if (err < 0)
		return err;

	err = usb_close(hnd);
	if (err < 0)
		return err;

	/* Open handle */
	hnd = usb_open(priv->dev);
	if (!hnd)
		return -1;

	err = usb_claim_interface(hnd, 0);
	if (err < 0)
		return err;

	//usb_set_altinterface(hnd, 1);

	priv->hnd = hnd;
	return 0;
}

static int
IOBoard_USBOps_Close_libusb(struct IOBoard *iob)
{
	struct IOBoardUSBPriv_libusb *priv;
	int err;

	priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;

	err = usb_release_interface(priv->hnd, 0);
	if (err < 0)
		return err;

	err = usb_close(priv->hnd);
	if (err < 0)
		return err;

	return 0;
}

static int
IOBoard_USBOps_ControlMsg_libusb(struct IOBoard *iob, int type, int req, int value,
				 int index, char *data, int size, int timeout)
{
	struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	return usb_control_msg(priv->hnd, type, req, value,
			       index, data, size, timeout);
}

static int
IOBoard_USBOps_BulkRead_libusb(struct IOBoard *iob, int endpoint, char *data, int len, int timeout)
{
	struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	return usb_bulk_read(priv->hnd, endpoint, data, len, timeout);
}

static int
IOBoard_USBOps_BulkWrite_libusb(struct IOBoard *iob, int endpoint, char *data, int len, int timeout)
{
	struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	return usb_bulk_write(priv->hnd, endpoint, data, len, timeout);
}

static int
IOBoard_USBOps_InterruptRead_libusb(struct IOBoard *iob, int endpoint, char *data, int len, int timeout)
{
	struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	return usb_interrupt_read(priv->hnd, endpoint, data, len, timeout);
}

static int
IOBoard_USBOps_InterruptWrite_libusb(struct IOBoard *iob, int endpoint, char *data, int len, int timeout)
{
	struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	return usb_interrupt_write(priv->hnd, endpoint, data, len, timeout);
}

static int
IOBoard_USBOps_Reset_libusb(struct IOBoard *iob)
{
	struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	return usb_reset(priv->hnd);
}

static int
IOBoard_USBOps_GetProductId_libusb(struct IOBoard *iob)
{
	struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	return priv->dev->descriptor.idProduct;
}

static struct IOBoardUSBOps IOBoardUSBOps_libusb = {
	.Open = IOBoard_USBOps_Open_libusb,
	.Close = IOBoard_USBOps_Close_libusb,
	.ControlMsg = IOBoard_USBOps_ControlMsg_libusb,
	.BulkRead = IOBoard_USBOps_BulkRead_libusb,
	.BulkWrite = IOBoard_USBOps_BulkWrite_libusb,
	.InterruptRead = IOBoard_USBOps_InterruptRead_libusb,
	.InterruptWrite = IOBoard_USBOps_InterruptWrite_libusb,
	.Reset = IOBoard_USBOps_Reset_libusb,
	.GetProductId = IOBoard_USBOps_GetProductId_libusb,
};

struct IOBoard *
IOBoard_Probe_libusb(int type)
{
	struct usb_bus *busses;
	struct usb_bus *bus;
	struct usb_device *usb_dev = NULL;
	struct IOBoard *iob = NULL;

	usb_find_busses();
	usb_find_devices();

	busses = usb_get_busses();

	for (bus = busses; bus; bus = bus->next) {
		struct usb_device *dev;
		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor == RPIMS_VENDOR_ID) {
				if (type < 0) {
					usb_dev = dev;
					goto found;
				} else if (dev->descriptor.idProduct == type) {
					usb_dev = dev;
					goto found;
				}
			}
		}
	}

 found:
	if (!usb_dev)
		return NULL;

	iob = (struct IOBoard *) malloc(sizeof(struct IOBoard));
	if (!iob)
		return NULL;

	/* Set USB operations */
	iob->usbops = &IOBoardUSBOps_libusb;
	iob->usbpriv = malloc(sizeof(struct IOBoardUSBPriv_libusb));
	if (!iob->usbpriv) {
		free(iob);
		return NULL;
	}
	((struct IOBoardUSBPriv_libusb*) iob->usbpriv)->dev = usb_dev;

	return iob;
}
