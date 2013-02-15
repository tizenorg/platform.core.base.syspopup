/*
 * syspopup
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Jayoun Lee <airjany@samsung.com>, Sewook Park <sewook7.park@samsung.com>,
 * Jaeho Lee <jaeho81.lee@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <aul.h>

#include "syspopup_core.h"
#include "syspopup_db.h"
#include "syspopup_api.h"
#include "simple_util.h"

API int syspopup_launch(char *popup_name, bundle *b)
{
	syspopup_info_t *info = NULL;
	int ret;
	int is_bundle = 0;

	if (popup_name == NULL) {
		_E("popup_name is NULL");
		return -1;
	}

	info = _syspopup_info_get(popup_name);
	if (info == NULL || info->pkgname == NULL) {
		_E("info or info->pkgname is NULL");
		if (info)
			_syspopup_info_free(info);
		return -1;
	}

	if (b == NULL) {
		b = bundle_create();
		is_bundle = 1;
	}

	if (_syspopup_set_name_to_bundle(b, popup_name) < 0) {
		_E("bundle set error\n");
		_syspopup_info_free(info);

		if (is_bundle == 1) {
			bundle_free(b);
		}
		return -1;
	}

	ret = aul_launch_app(info->pkgname, b);
	if (ret < 0) {
		_E("aul launch error - %d", ret);
	}

	if (is_bundle == 1) {
		bundle_free(b);
	}

	_syspopup_info_free(info);

	return ret;
}

API int syspopup_destroy_all()
{
	DBusMessage *message;
	DBusError error;
	DBusConnection *bus = NULL;

	dbus_error_init(&error);
	bus = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);
	if (bus == NULL) {
		_E("Failed to connect to the D-BUS daemon: %s", error.message);
		dbus_error_free(&error);
		return -1;
	}

	message = dbus_message_new_signal(SYSPOPUP_DBUS_PATH,
					  SYSPOPUP_DBUS_SIGNAL_INTERFACE,
					  SYSPOPUP_DBUS_SP_TERM_SIGNAL);

	if (dbus_connection_send(bus, message, NULL) == FALSE) {
		_E("dbus send error");
		return -1;
	}

	dbus_connection_flush(bus);
	dbus_message_unref(message);

	dbus_connection_close(bus);

	_D("send signal done\n");

	return 0;
}

