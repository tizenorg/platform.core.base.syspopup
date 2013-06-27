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


#include "syspopup_core.h"
#include "syspopup.h"
#include "syspopup_api.h"
#include "simple_util.h"
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Input.h>

#ifndef WAYLAND
#include <Ecore_X.h>
#endif

#define WIN_PROP_NAME "SYSTEM_POPUP"

static void __elm_popupwin_del_cb(void *data, Evas * e, Evas_Object * obj,
				  void *event_info)
{
	int id;

	id = (int)data;
	_D("callback del called, destroy internal data - id = %d\n", id);

	_syspopup_del(id);
}

static Eina_Bool __x_keydown_cb(void *data, int type, void *event)
{
	int id = (int)data;
	Ecore_Event_Key *ev = event;

	if (ev == NULL)
		return 0;

	X_syspopup_process_keydown(id, ev->keyname);

	return ECORE_CALLBACK_DONE;
}

#ifdef ROTATE_USING_X_CLIENT
static Eina_Bool __x_rotate_cb(void *data, int type, void *event)
{
	int id = (int)data;

#ifndef WAYLAND
	Ecore_X_Event_Client_Message *ev = event;

	if (!event)
		return ECORE_CALLBACK_RENEW;

	if (ev->message_type == ECORE_X_ATOM_E_ILLUME_ROTATE_ROOT_ANGLE)
		X_syspopup_process_rotate(id);
#endif

	return ECORE_CALLBACK_RENEW;
}

#ifndef WAYLAND
static int __efl_rotate(Display *dpy, Window win, syspopup *sp)
{
	int rotation;

	rotation = X_syspopup_rotation_get(dpy, win);

	if (rotation == -1) {
		rotation = 0;
	}

	if (rotation >= 0)
		elm_win_rotation_with_resize_set(sp->win, rotation);

	return 0;
}
#endif
#else
static int __efl_rotate(Display *dpy, Window win, syspopup *sp)
{
	return 0;
}
#endif

API int syspopup_create(bundle *b, syspopup_handler *handler,
			Evas_Object *parent, void *user_data)
{
#ifndef WAYLAND
	Ecore_X_Window xwin;
	Display *dpy;
	const char *popup_name;
	syspopup *sp = NULL;
	int id;
	XWindowAttributes attr;
	int is_unviewable = 0;

	popup_name = _syspopup_get_name_from_bundle(b);
	if (popup_name == NULL || handler == NULL) {
		_E("popup_name or handler is NULL");
		return -1;
	}

	if (parent == NULL) {
		_E("parent window is NULL");
		return -1;
	}

	sp = _syspopup_find(popup_name);
	if (sp) {
		_E("already exist - syspopup %s", popup_name);
		return -1;
	} else {
		xwin = elm_win_xwindow_get(parent);
		dpy = ecore_x_display_get();

		id = X_make_syspopup(b, dpy, xwin, parent, __efl_rotate,
				     handler, user_data);
		if (id < 0) {
			_E("fail to make X syspopup");
			return -1;
		}

		ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, __x_keydown_cb,
					(void *)id);

		evas_object_event_callback_add(parent, EVAS_CALLBACK_DEL,
					 __elm_popupwin_del_cb, (void *)id);

		/* X_syspopup_core should process 2 events */
		/* First, rotate event */
		/* Second, keydown event */
#ifdef ROTATE_USING_X_CLIENT
		ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE,__x_rotate_cb, (void *)id);
#else
		ecore_x_icccm_name_class_set(xwin, WIN_PROP_NAME, WIN_PROP_NAME);
		if (elm_win_wm_rotation_supported_get(parent)) {
			int rots[4] = { 0, 90, 180, 270 };
			elm_win_wm_rotation_available_rotations_set(parent, &rots, 4);
		} else {
			_E("win rotation no supported");
		}
#endif
	}
#endif

	return 0;
}

API int syspopup_reset(bundle *b)
{
	return X_syspopup_reset(b);
}

