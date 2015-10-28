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

#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <grp.h>
#include <gio/gio.h>
#include <bundle.h>
#include <bundle_internal.h>

/* For multi-user support */
#include <tzplatform_config.h>

#include "syspopup_core.h"
#include "simple_util.h"

#define SYSPOPUP_NAME "_INTERNAL_SYSPOPUP_NAME_"

static syspopup *syspopup_head = NULL;

static int initialized = 0;
static int sp_id = 0;

static void (*_term_handler)(void *data);
static gboolean (*_timeout_handler)(gpointer data);

syspopup *_syspopup_get_head(void)
{
	return syspopup_head;
}

int _syspopup_add_new(syspopup *sp)
{
	if (sp == NULL)
		return -1;

	sp->id = sp_id++;
	sp->next = syspopup_head;
	syspopup_head = sp;

	return 0;
}

syspopup *_syspopup_find(const char *name)
{
	syspopup *tmp;

	tmp = syspopup_head;
	while (tmp) {
		if (tmp->name) {
			if (strcmp(tmp->name, name) == 0)
				return tmp;
		}

		tmp = tmp->next;
	}

	return NULL;
}

syspopup *_syspopup_find_by_id(int id)
{
	syspopup *tmp;

	tmp = syspopup_head;
	while (tmp) {
		if (tmp->id == id)
			return tmp;

		tmp = tmp->next;
	}

	return NULL;
}

static void __syspopup_free(syspopup *sp)
{
	if (sp->name != NULL)
		free(sp->name);

	if (sp->dupped_bundle != NULL)
		bundle_free(sp->dupped_bundle);

	free(sp);
}

void _syspopup_del(int id)
{
	syspopup *tmp;
	syspopup *target;

	target = _syspopup_find_by_id(id);

	if (syspopup_head == NULL || target == NULL)
		return;

	if (syspopup_head == target) {
		syspopup_head = target->next;
		__syspopup_free(target);
		return;
	}

	tmp = syspopup_head;
	while (tmp) {
		if (tmp->next == target) {
			tmp->next = target->next;
			__syspopup_free(target);
			return;
		}

		tmp = tmp->next;
	}
}

static void __syspopup_dbus_signal_filter(GDBusConnection *conn,
					const gchar *sender_name,
					const gchar *object_path,
					const gchar *interface_name,
					const gchar *signal_name,
					GVariant *parameters,
					gpointer user_data)
{
	if (signal_name
		&& strcmp(signal_name, SYSPOPUP_DBUS_SP_TERM_SIGNAL) == 0) {
		if (_term_handler)
			_term_handler(NULL);

		_D("term handler has been called");
	}
}

int _syspopup_init(void (*term_handler)(void *),
		gboolean (*timeout_handler)(gpointer))
{
	GDBusConnection *conn = NULL;
	GError *err = NULL;
	guint conn_subsc_id;

	if (initialized)
		return 0;

#if !(GLIB_CHECK_VERSION(2, 36, 0))
	g_type_init();
#endif

	_term_handler = term_handler;
	_timeout_handler = timeout_handler;

	conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &err);
	if (err) {
		_E("gdbus connection error (%s)", err->message);
		g_error_free(err);
		return -1;
	}

	/* Add a fileter for signal */
	conn_subsc_id = g_dbus_connection_signal_subscribe(conn,
						NULL,
						SYSPOPUP_DBUS_SIGNAL_INTERFACE,
						NULL,
						SYSPOPUP_DBUS_PATH,
						NULL,
						G_DBUS_SIGNAL_FLAGS_NONE,
						__syspopup_dbus_signal_filter,
						NULL,
						NULL);
	if (conn_subsc_id == 0) {
		_E("Error in subscribing to the signal");
		return -1;
	}

	_D("syspopup signal initialized");
	initialized = 1;

	return 0;
}

int _syspopup_reset_timeout(syspopup *sp, syspopup_info_t *info)
{
	if (!initialized) {
		_E("no initialized");
		return -1;
	}

	if (sp == NULL) {
		_E("syspopup is NULL");
		return -1;
	}

	if (info == NULL) {
		_E("syspopup info is NULL");
		return -1;
	}

	if (sp->timeout_id != 0)
		g_source_remove(sp->timeout_id);

	if (info->timeout > 0) {
		sp->timeout_id = g_timeout_add_seconds(info->timeout,
					_timeout_handler, (gpointer)sp->id);
		_D("add timeout - timeout : id=%d,timeout=%d(sec)",
					sp->id, info->timeout);
	}

	return 0;
}

int _syspopup_set_term_type(syspopup *sp, syspopup_info_t *info)
{
	sp->term_act = info->term_act;
	return 0;
}

int _syspopup_set_endkey_type(syspopup *sp, syspopup_info_t *info)
{
	sp->endkey_act = info->endkey_act;
	return 0;
}

const char *_syspopup_get_name_from_bundle(bundle *b)
{
	const char *name;

	if (b == NULL) {
		_E("bundle is NULL");
		return NULL;
	}

	if (getuid() != 0 && getuid() != tzplatform_getuid(TZ_USER_NAME)) {
		_E("syspopup permission error");
		return NULL;
	}

	name = bundle_get_val(b, SYSPOPUP_NAME);
	if (name == NULL)
		_E("this is no bundle for syspopup");

	return name;
}

int _syspopup_set_name_to_bundle(bundle *b, char *popup_name)
{
	int ret;

	if (b == NULL || popup_name == NULL)
		return -1;

	ret = bundle_add(b, SYSPOPUP_NAME, popup_name);
	if (ret != BUNDLE_ERROR_NONE)
		return -1;

	return 0;
}
