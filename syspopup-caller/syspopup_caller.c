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
#include <gio/gio.h>
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
	if (ret < 0)
		_E("aul launch error: %d", ret);

	if (is_bundle == 1)
		bundle_free(b);

	_syspopup_info_free(info);

	return ret;
}

API int syspopup_destroy_all(void)
{
	GDBusConnection *conn = NULL;
	GError *err = NULL;
	int ret = 0;

#if !(GLIB_CHECK_VERSION(2, 36, 0))
	g_type_init();
#endif

	conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &err);
	if (err) {
		_E("gdbus connection error: %s", err->message);
		g_error_free(err);
		return -1;
	}

	if (g_dbus_connection_emit_signal(conn,
					NULL,
					SYSPOPUP_DBUS_PATH,
					SYSPOPUP_DBUS_SIGNAL_INTERFACE,
					SYSPOPUP_DBUS_SP_TERM_SIGNAL,
					NULL,
					&err) == FALSE) {
		_E("emitting the signal error: %s", err->message);
		ret = -1;
	} else {
		_D("send signal done");
	}

	if (err)
		g_error_free(err);
	if (conn)
		g_object_unref(conn);

	return ret;
}
