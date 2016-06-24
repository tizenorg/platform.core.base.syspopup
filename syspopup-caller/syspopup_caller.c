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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>
#include <gio/gio.h>
#include <bundle.h>
#include <systemd/sd-login.h>
#include <aul.h>

#include "syspopup_core.h"
#include "syspopup_db.h"
#include "syspopup_api.h"
#include "simple_util.h"

#define REGULAR_UID_MIN 5000

API int syspopup_launch_for_uid(char *popup_name, bundle *b, uid_t uid)
{
	syspopup_info_t *info;
	int ret = -1;
	int is_bundle = 0;
	int uid_max;
	int i;
	char *state = NULL;
	uid_t *uids = NULL;
	uid_t cuid = getuid();

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
		if (is_bundle == 1)
			bundle_free(b);

		return -1;
	}

	if (cuid >= REGULAR_UID_MIN) {
		if (cuid != uid) {
			_E("Cannot launch syspopup %s in other users",
					popup_name);
			ret = -1;
			goto end;
		}

		ret = aul_launch_app_for_uid(info->pkgname, b, uid);
		if (ret < 0)
			_E("syspopup launch error - %d", ret);
	} else {
		if (uid >= REGULAR_UID_MIN) {
			ret = aul_launch_app_for_uid(info->pkgname, b, uid);
			if (ret < 0)
				_E("syspopup launch error - %d", ret);
		} else {
			uid_max = sd_get_uids(&uids);
			if (uid_max <= 0) {
				_E("Failed to get uid list");
				ret = -1;
				goto end;
			}

			for (i = 0; i < uid_max; i++) {
				if (sd_uid_get_state(uids[i], &state) < 0)
					continue;

				if (state && !strcmp(state, "online")) {
					ret = aul_launch_app_for_uid(
							info->pkgname,
							b, uids[i]);
					if (ret < 0) {
						_E("syspopup launch error - %d",
								ret);
					}
				}
			}
		}
	}

end:
	if (is_bundle)
		bundle_free(b);

	_syspopup_info_free(info);

	return ret > 0 ? 0 : -1;
}

API int syspopup_launch(char *popup_name, bundle *b)
{
	return syspopup_launch_for_uid(popup_name, b, getuid());
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
