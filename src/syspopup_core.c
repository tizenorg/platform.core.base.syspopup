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
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <sys/types.h>
#include <grp.h>

/* For multi-user support */
#include <tzplatform_config.h>

#include "syspopup_core.h"
#include "simple_util.h"

#define SYSPOPUP_NAME "_INTERNAL_SYSPOPUP_NAME_"

static syspopup *syspopup_head = NULL;

static int initialized = 0;
static DBusConnection *bus;
static int noti_fd = -1;
static int sp_id = 0;

static void (*_term_handler) (void *data);
static gboolean(*_timeout_handler) (void *data);

syspopup *_syspopup_get_head()
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


static DBusHandlerResult
__sys_popup_dbus_signal_filter(DBusConnection *conn, DBusMessage *message,
				     void *user_data)
{
	const char *sender;
	const char *interface;
	int dead_pid;

	DBusError error;
	dbus_error_init(&error);

	interface = dbus_message_get_interface(message);
	if (interface == NULL) {
		_E("reject by security issue - no interface\n");
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	if (dbus_message_is_signal(message, interface,
				   SYSPOPUP_DBUS_SP_TERM_SIGNAL)) {
		if (_term_handler)
			_term_handler(NULL);

		_D("term handler has been called");
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}



int _syspopup_init(void (*term_handler) (void *),
		   gboolean(*timeout_handler) (void *))
{
	DBusError error;
	char rule[MAX_LOCAL_BUFSZ];

	if (initialized)
		return 0;

	_term_handler = term_handler;
	_timeout_handler = timeout_handler;

	dbus_error_init(&error);
	bus = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);
	if (!bus) {
		_E("Failed to connect to the D-BUS daemon: %s", error.message);
		dbus_error_free(&error);
		return -1;
	}
	dbus_connection_setup_with_g_main(bus, NULL);

	snprintf(rule, MAX_LOCAL_BUFSZ,
		 "path='%s',type='signal',interface='%s'", SYSPOPUP_DBUS_PATH,
		 SYSPOPUP_DBUS_SIGNAL_INTERFACE);
	/* listening to messages */
	dbus_bus_add_match(bus, rule, &error);
	if (dbus_error_is_set(&error)) {
		_E("Fail to rule set: %s", error.message);
		dbus_error_free(&error);
		return -1;
	}

	if (dbus_connection_add_filter(bus, 
		__sys_popup_dbus_signal_filter, NULL, NULL) == FALSE)
		return -1;

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

	if(info->timeout > 0) {
		sp->timeout_id = g_timeout_add_seconds(info->timeout, _timeout_handler,
					       (void *)sp->id);
		_D("add timeout - timeout : id=%d,timeout=%d(sec)", sp->id, 
			info->timeout);
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

	if (getuid() != 0 && getuid() != tzplatform_getuid(TZ_USER_NAME)) {
		_E("syspopup permission error");
		return NULL;
	}

	if (b == NULL) {
		_E("bundle is NULL");
		return NULL;
	}

	name = bundle_get_val(b, SYSPOPUP_NAME);
	if (name == NULL) {
		_E("this is no bundle for syspopup");
	}
	return name;
}

int _syspopup_set_name_to_bundle(bundle *b, char *popup_name)
{
	if ((b == NULL) || (popup_name == NULL))
		return -1;
	bundle_add(b, SYSPOPUP_NAME, popup_name);
	return 0;
}

