/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2011  Nokia Corporation
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <glib.h>
#include <btio/btio.h>
#include <stdlib.h>
#include <stdint.h>

#include "grib_ble_extend.h"

int main_command(int argc, char *argv[]);
gboolean listen_start(gpointer user_data);
gboolean characteristics_write_req(gpointer user_data);

int interactive(const gchar *src, const gchar *dst, const gchar *dst_type, int psm);
GIOChannel *gatt_connect(const gchar *src, const gchar *dst,
			const gchar *dst_type, const gchar *sec_level,
			int psm, int mtu, BtIOConnect connect_cb);

//2 shbaek: for My Control.
GIOChannel *gatt_connectEx(const gchar *src, const gchar *dst,
				const gchar *dst_type, const gchar *sec_level,
				int psm, int mtu, BtIOConnect connect_cb, 
				gpointer user_data, GDestroyNotify destroyCb, GError **err);

size_t gatt_attr_data_from_string(const char *str, uint8_t **data);
