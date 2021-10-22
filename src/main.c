/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <zephyr.h>
#include <stddef.h>
#include <sys/printk.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

//#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME "Such a very long name of our product"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/*
 * Set Advertisement data. Based on the Eddystone specification:
 * https://github.com/google/eddystone/blob/master/protocol-specification.md
 * https://github.com/google/eddystone/tree/master/eddystone-url
 */
char d1[] = "Alpha Project";

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xaa, 0xfe),
	BT_DATA_BYTES(BT_DATA_SVC_DATA16,
		      0xaa, 0xfe, /* Eddystone UUID */
		      0x10, /* Eddystone-URL frame type */
		      0x00, /* Calibrated Tx power at 0m */
		      0x00, /* URL Scheme Prefix http://www. */
		      'T', 'L', 'P', 'H', 'A', 'r',
		      'p', 'r', 'o', 'j', 'e', 'c', 't',
		      0x08) /* .org */
};

/* Set Scan Response data */
static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void bt_ready(int err)
{
	char addr_s[BT_ADDR_LE_STR_LEN];
	bt_addr_le_t addr = {0};
	size_t count = 1;

	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	char arr[5][25] =
			{ "This is a first rolling key",
			"Maybe we should switch to this one",
			"Ugh I forgot the limitations",
			"tell the array size",
			"tell it again bro!"
			};

	printk("Starting Beacon Demo\n");

se	for (int i=0; i<5; i++){
		printk("Bluetooth initialized\n");
		printk("BLE Name is %s \n", arr[i]);

		struct bt_data sd[] = {BT_DATA(BT_DATA_NAME_COMPLETE, arr[i], DEVICE_NAME_LEN)};

		/* Start advertising */
		err = bt_le_adv_start(BT_LE_ADV_NCONN_IDENTITY, ad, ARRAY_SIZE(ad),
					sd, ARRAY_SIZE(sd));
		k_msleep(10000);
		printk("Bluetooth stopped\n");
		int err2 = bt_le_adv_stop();
		if (err2)
			printk("Error on stop");
		#undef DEVICE_NAME
		#define DEVICE_NAME arr[i]

	}

	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}


	/* For connectable advertising you would use
	 * bt_le_oob_get_local().  For non-connectable non-identity
	 * advertising an non-resolvable private address is used;
	 * there is no API to retrieve that.
	 */

	bt_id_get(&addr, &count);
	bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));

	printk("Beacon started, advertising as %s\n", addr_s);
}

void main(void)
{
	int err;
	
	err = bt_enable(bt_ready);
		
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}
}
