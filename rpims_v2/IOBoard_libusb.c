/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <libusb.h>
#include <usb.h>

#include <stdio.h>

#include "IOBoard.h"

struct IOBoardUSBPriv_libusb {
	struct libusb_device *dev;
	struct libusb_device_handle *hnd;
};

static int
IOBoard_USBOps_Open_libusb(struct IOBoard *iob) {

	struct IOBoardUSBPriv_libusb *priv;
	struct libusb_device_handle *hnd;
	int err;

	priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;

	/* Reset USB device */
	err = libusb_open(priv->dev, &hnd);
	if (!err == 0) {
		return -1;
	}

	err = libusb_reset_device(hnd);
	if (err < 0) {
		return err;
	}

	libusb_close(hnd);
	if (err < 0) {
		return err;
	}

	/* Open handle */
	err = libusb_open(priv->dev, &hnd);
	if (!err == 0) {
		return -1;
	}

	err = libusb_claim_interface(hnd, 0);
	if (err < 0) {
		return err;
	}

	//usb_set_altinterface(hnd, 1);

	priv->hnd = hnd;
	return 0;
}

static int
IOBoard_USBOps_Close_libusb(struct IOBoard *iob) {
	struct IOBoardUSBPriv_libusb *priv;
	int err;

	priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;

	err = libusb_release_interface(priv->hnd, 0);
	if (err < 0)
		return err;

	libusb_close(priv->hnd);
	if (err < 0)
		return err;

	return 0;
}

static int
IOBoard_USBOps_ControlMsg_libusb(struct IOBoard *iob, int type, int req, int value,
				 int index, char *data, int size, int timeout) {
	// struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	// return usb_control_msg(priv->hnd, type, req, value,
			    //    index, data, size, timeout);
				return 0;
}

static int
IOBoard_USBOps_BulkRead_libusb(struct IOBoard *iob, int endpoint, char *data, int len, int timeout) {
	// struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	// return usb_bulk_read(priv->hnd, endpoint, data, len, timeout);
	return 0;
}

static int
IOBoard_USBOps_BulkWrite_libusb(struct IOBoard *iob, int endpoint, char *data, int len, int timeout) {
	// struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	// return usb_bulk_write(priv->hnd, endpoint, data, len, timeout);
	return 0;
}

static int
IOBoard_USBOps_InterruptRead_libusb(struct IOBoard *iob, int endpoint, char *data, int len, int timeout) {
	// struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	// return usb_interrupt_read(priv->hnd, endpoint, data, len, timeout);
	return 0;
}

static int
IOBoard_USBOps_InterruptWrite_libusb(struct IOBoard *iob, int endpoint, char *data, int len, int timeout) {
	// struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	// return usb_interrupt_write(priv->hnd, endpoint, data, len, timeout);
	return 0;
}

static int
IOBoard_USBOps_Reset_libusb(struct IOBoard *iob) {
	struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
	return libusb_reset_device(priv->hnd);
}

static int
IOBoard_USBOps_GetProductId_libusb(struct IOBoard *iob) {
	struct IOBoardUSBPriv_libusb *priv = (struct IOBoardUSBPriv_libusb *) iob->usbpriv;
    struct libusb_device_descriptor desc = {0};
	// todo check return code
	libusb_get_device_descriptor(priv->dev, &desc);
    return desc.idProduct;
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

struct IOBoard * IOBoard_Probe_libusb(int type) {
	struct libusb_device **libusb_devs = NULL;
	struct libusb_device *libusb_dev = NULL;
	struct IOBoard *iob = NULL;

	ssize_t count = libusb_get_device_list(NULL, &libusb_devs);

	for (size_t idx = 0; libusb_devs[idx] != NULL; idx++) {
		struct libusb_device *dev = libusb_devs[idx];
		struct libusb_device_descriptor desc;

		libusb_get_device_descriptor(libusb_dev, &desc);
		printf("%x\n", desc.idVendor);
		if (desc.idVendor == RPIMS_VENDOR_ID) {
			if (type < 0) {
				libusb_dev = dev;
				goto found;
			} else if (desc.idProduct == type) {
				libusb_dev = dev;
				goto found;
			}
		}
	}


 found:
	if (!libusb_dev) {
		return NULL;
	}

	printf("Got a device!\n");
	iob = (struct IOBoard *) malloc(sizeof(struct IOBoard));

	if (!iob) {
		return NULL;
	}
	printf("Got a struct!\n");

	/* Set USB operations */
	iob->usbops = &IOBoardUSBOps_libusb;
	iob->usbpriv = malloc(sizeof(struct IOBoardUSBPriv_libusb));
	if (!iob->usbpriv) {
		free(iob);
		return NULL;
	}
	printf("Got a usbpriv!!\n");
	((struct IOBoardUSBPriv_libusb*) iob->usbpriv)->dev = libusb_dev;

	if (!iob) {
		printf("something's rotten in the state of denmark");
	}

	libusb_free_device_list(libusb_devs, count);
	return iob;
}
