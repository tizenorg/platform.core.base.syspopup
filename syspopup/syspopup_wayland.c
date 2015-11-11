/*
 * syspopup wayland
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd. All rights reserved.
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

#include <Ecore_Wayland.h>
#include <efl_util.h>
#include <Ecore.h>

#include "syspopup.h"
#include "syspopup_wayland.h"
#include "simple_util.h"

static void __wl_syspopup_term_handler(gpointer data, gpointer user_data)
{
	syspopup *sp = data;

	if (sp == NULL)
		return;

	_D("term action %d - %s", sp->term_act, sp->name);

	switch (sp->term_act) {
	case SYSPOPUP_TERM:
		if (sp->def_term_fn)
			sp->def_term_fn(sp->dupped_bundle, sp->user_data);
		break;
	case SYSPOPUP_HIDE:
		if (sp->def_term_fn)
			sp->def_term_fn(sp->dupped_bundle, sp->user_data);
		ecore_wl_window_hide((Ecore_Wl_Window *)sp->internal_data);
		break;
	default:
		_D("term action IGNORED: %s", sp->name);
		break;
	}
}

static gboolean __wl_syspopup_timeout_handler(gpointer user_data)
{
	syspopup *sp;
	int id = (int)((intptr_t)user_data);

	sp = _syspopup_find_by_id(id);
	if (sp == NULL) {
		_E("Failed to find the syspopup");
		return FALSE;
	}

	_D("find timeout - %s", sp->name);
	if (sp->def_timeout_fn)
		sp->def_timeout_fn(sp->dupped_bundle, sp->user_data);

	return FALSE;
}

static void __wl_rotation_set(syspopup *sp)
{
	int rots[] = {0, 90, 180, 270};

	ecore_wl_window_class_name_set(sp->internal_data, WIN_PROP_NAME);
	if (elm_win_wm_rotation_supported_get(sp->win))
		elm_win_wm_rotation_available_rotations_set(sp->win, rots, 4);
	else
		_E("win rotation is not supported");
}

static efl_util_notification_level_e __wl_syspopup_get_notification_level(int priority)
{
	switch (priority) {
	case 0:
		return EFL_UTIL_NOTIFICATION_LEVEL_DEFAULT;
	case 1:
		return EFL_UTIL_NOTIFICATION_LEVEL_MEDIUM;
	case 2:
		return EFL_UTIL_NOTIFICATION_LEVEL_HIGH;
	default:
		return EFL_UTIL_NOTIFICATION_LEVEL_DEFAULT;
	}
}

int wl_syspopup_init(syspopup *sp, syspopup_info_t *info)
{
	Ecore_Wl_Window *wl_win;
	efl_util_notification_level_e level;

	if (_syspopup_init(__wl_syspopup_term_handler,
				__wl_syspopup_timeout_handler) < 0)
		return -1;

	wl_win = (Ecore_Wl_Window *)sp->internal_data;
	ecore_wl_window_type_set(wl_win, ECORE_WL_WINDOW_TYPE_NOTIFICATION);

	level = __wl_syspopup_get_notification_level(info->prio);
	efl_util_set_notification_window_level(sp->win, level);
	if (info->focus)
		ecore_wl_window_focus_skip_set(wl_win, EINA_TRUE);

	__wl_rotation_set(sp);

	return 0;
}

int wl_syspopup_reset(bundle *b)
{
	const char *popup_name;
	syspopup_info_t *info;
	syspopup *sp;
	Ecore_Wl_Window *wl_win;
	efl_util_notification_level_e level;

	popup_name = _syspopup_get_name_from_bundle(b);
	if (popup_name == NULL)
		return -1;

	sp = _syspopup_find(popup_name);
	if (sp == NULL)
		return -1;

	info = _syspopup_info_get(popup_name);
	if (info == NULL)
		return -1;

	_syspopup_reset_timeout(sp, info);
	if (sp->dupped_bundle)
		free(sp->dupped_bundle);

	sp->dupped_bundle = bundle_dup(b);

	wl_win = (Ecore_Wl_Window *)sp->internal_data;
	ecore_wl_window_type_set(wl_win, ECORE_WL_WINDOW_TYPE_NOTIFICATION);

	level = __wl_syspopup_get_notification_level(info->prio);
	efl_util_set_notification_window_level(sp->win, level);
	if (info->focus)
		ecore_wl_window_focus_skip_set(wl_win, EINA_TRUE);

	__wl_rotation_set(sp);

	_syspopup_info_free(info);

	return 0;
}

int wl_syspopup_process_keypress(int id, const char *keyname)
{
	syspopup *sp;

	if (keyname == NULL)
		return 0;

	_D("key press - %s", keyname);

	if (strcmp(keyname, KEY_END) != 0)
		return 0;

	sp = _syspopup_find_by_id(id);
	if (sp == NULL)
		return 0;

	_D("find key down - %s", sp->name);
	if (sp->endkey_act == SYSPOPUP_KEYEND_TERM) {
		if (sp->def_term_fn)
			sp->def_term_fn(sp->dupped_bundle, sp->user_data);
	} else if (sp->endkey_act == SYSPOPUP_KEYEND_HIDE) {
		if (sp->def_term_fn)
			sp->def_term_fn(sp->dupped_bundle, sp->user_data);

		ecore_wl_window_hide((Ecore_Wl_Window *)sp->internal_data);
	} else {
		_E("no find key down");
	}

	return 0;
}
