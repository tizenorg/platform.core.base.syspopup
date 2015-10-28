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

#include <utilX.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <Ecore_X.h>

#include "syspopup.h"
#include "syspopup_x.h"
#include "simple_util.h"

static void __x_rotation_set(Display *dpy, Window win, syspopup *sp);

static void __x_syspopup_term_handler(void *data)
{
	syspopup *tmp;
	Display *dpy;
	Window win;

	_D("enter syspopup term handler");

	dpy = XOpenDisplay(NULL);

	tmp = _syspopup_get_head();
	while (tmp) {
		_D("term action %d - %s", tmp->term_act, tmp->name);

		switch (tmp->term_act) {
		case SYSPOPUP_TERM:
			win = (Window)tmp->internal_data;

			if (tmp->def_term_fn)
				tmp->def_term_fn(tmp->dupped_bundle,
						 tmp->user_data);

			XKillClient(dpy, win);
			break;
		case SYSPOPUP_HIDE:
			win = (Window)tmp->internal_data;

			if (tmp->def_term_fn)
				tmp->def_term_fn(tmp->dupped_bundle,
						 tmp->user_data);
			XUnmapWindow(dpy, win);
			break;
		default:
			_D("term action IGNORED: %s", tmp->name);
		}

		tmp = tmp->next;
	}

	XCloseDisplay(dpy);
}

static gboolean __x_syspopup_timeout_handler(gpointer user_data)
{
	int id = (int)user_data;
	syspopup *sp;
	Display *dpy;
	Window win;

	sp = _syspopup_find_by_id(id);
	if (sp == NULL) {
		_E("no find timeout");
		return FALSE;
	}

	dpy = XOpenDisplay(NULL);

	_D("find timeout - %s", sp->name);
	if (sp->def_timeout_fn != NULL)
		sp->def_timeout_fn(sp->dupped_bundle, sp->user_data);

	win = (Window)sp->internal_data;
	XKillClient(dpy, win);
	XCloseDisplay(dpy);

	return FALSE;
}

static int __x_syspopup_change_xwin_type(Display *dpy, Window win)
{
	Atom win_type_atom;
	Atom win_type_utility_atom;

	win_type_atom = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
	win_type_utility_atom =
	    XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_NOTIFICATION", False);
	XChangeProperty(dpy, win, win_type_atom, XA_ATOM, 32, PropModeReplace,
			(unsigned char *)&win_type_utility_atom, 1);

	return 0;
}

static int __x_syspopup_disable_focus(Display *dpy, Window win)
{
	XWMHints *hints;

	hints = XAllocWMHints();
	if (hints == NULL)
		return -1;

	hints->flags = InputHint | StateHint;
	hints->input = 0;
	hints->initial_state = NormalState;

	XSetWMHints(dpy, win, hints);
	XFree(hints);

	return 0;
}

#ifdef ROTATE_USING_X_CLIENT
static int __utilx_ss_get_window_property(Display *dpy, Window win, Atom atom,
					  Atom type, unsigned int *val,
					  unsigned int len)
{
	unsigned char *prop_ret;
	Atom type_ret;
	unsigned long bytes_after;
	unsigned long  num_ret;
	int format_ret;
	unsigned int i;
	int num;

	prop_ret = NULL;
	if (XGetWindowProperty(dpy, win, atom, 0, 0x7fffffff, False,
			       type, &type_ret, &format_ret, &num_ret,
			       &bytes_after, &prop_ret) != Success)
		return -1;

	if (type_ret != type || format_ret != 32)
		num = -1;
	else if (num_ret == 0 || !prop_ret)
		num = 0;
	else {
		if (num_ret < len)
			len = num_ret;

		for (i = 0; i < len; i++)
			val[i] = ((unsigned long *)prop_ret)[i];

		num = len;
	}

	if (prop_ret)
		XFree(prop_ret);

	return num;
}

static int x_syspopup_rotation_get(Display *dpy, Window win)
{
	Window active_win;
	Window root_win;
	Atom atom_active_win;
	Atom atom_win_rotate_angle;
	int rotation = -1;
	int ret;

	root_win = XDefaultRootWindow(dpy);

	atom_active_win = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
	ret = __utilx_ss_get_window_property(dpy, root_win, atom_active_win,
					     XA_WINDOW,
					     (unsigned int *)&active_win, 1);
	if (ret < 0)
		return ret;

	atom_win_rotate_angle =	XInternAtom(dpy,
					"_E_ILLUME_ROTATE_ROOT_ANGLE", False);
	ret = __utilx_ss_get_window_property(dpy, root_win,
					  atom_win_rotate_angle, XA_CARDINAL,
					  (unsigned int *)&rotation, 1);
	if (ret != -1)
		return rotation;

	return -1;
}

static int x_syspopup_process_rotate(int id)
{
	Display *dpy;
	Window win;
	syspopup *sp;

	sp = _syspopup_find_by_id(id);
	if (sp == NULL)
		return -1;

	win = (Window) sp->internal_data;
	dpy = XOpenDisplay(NULL);
	__x_rotation_set(dpy, win, sp);
	XCloseDisplay(dpy);

	return 0;
}

static Eina_Bool __x_rotate_cb(void *data, int type, void *event)
{
	int id = (int)data;
	Ecore_X_Event_Client_Message *ev = event;

	if (ev && ev->message_type == ECORE_X_ATOM_E_ILLUME_ROTATE_ROOT_ANGLE)
		x_syspopup_process_rotate(id);

	return ECORE_CALLBACK_RENEW;
}

static void __x_rotation_set(Display *dpy, Window win, syspopup *sp)
{
	int rotation;

	rotation = x_syspopup_rotation_get(dpy, win);
	if (rotation == -1)
		rotation = 0;

	elm_win_rotation_with_resize_set(sp->win, rotation);

	return 0;
}
#else
static void __x_rotation_set(Display *dpy, Window win, syspopup *sp)
{
	int rots[] = {0, 90, 180, 270};

	ecore_x_icccm_name_class_set(win, WIN_PROP_NAME, WIN_PROP_NAME);
	if (elm_win_wm_rotation_supported_get(sp->win))
		elm_win_wm_rotation_available_rotations_set(sp->win, rots, 4);
	else
		_E("win rotation no supported");
}
#endif

int x_syspopup_init(syspopup *sp, syspopup_info_t *info)
{
	Ecore_X_Window xwin;
	Display *dpy;
	XWindowAttributes attr;
	int is_unviewable = 0;

	if (_syspopup_init(__x_syspopup_term_handler,
				__x_syspopup_timeout_handler) < 0)
		return -1;

	xwin = (Ecore_X_Window)sp->internal_data;
	dpy = ecore_x_display_get();
	XGetWindowAttributes(dpy, xwin, &attr);
	if (attr.map_state == IsViewable) {
		XUnmapWindow(dpy, xwin);
		is_unviewable = 1;
	}

	__x_syspopup_change_xwin_type(dpy, xwin);
	utilx_set_system_notification_level(dpy, xwin, info->prio);
	if (info->focus)
		__x_syspopup_disable_focus(dpy, xwin);

	__x_rotation_set(dpy, xwin, sp);

	if (is_unviewable)
		XMapWindow(dpy, xwin);

#ifdef ROTATE_USING_X_CLIENT
	ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE,
				__x_rotate_cb, (void *)sp->id);
#endif

	return 0;
}

int x_syspopup_reset(bundle *b)
{
	const char *popup_name;
	syspopup_info_t *info;
	syspopup *sp;
	Display *d;
	Window win;

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

	do {
		d = XOpenDisplay(NULL);
		if (d == NULL)
			break;

		win = (Window)sp->internal_data;
		if (win == NULL)
			break;

		utilx_set_system_notification_level(d, win, info->prio);

		if (info->focus)
			__x_syspopup_disable_focus(d, win);

		__x_rotation_set(d, win, sp);
		XMapWindow(d, win);
		XCloseDisplay(d);
	} while (0);

	_syspopup_info_free(info);

	return 0;
}

int x_syspopup_process_keypress(int id, const char *keyname)
{
	Display *dpy;
	Window win;
	syspopup *sp = NULL;

	if (keyname == NULL)
		return 0;

	_D("key press - %s", keyname);

	if (strcmp(keyname, KEY_END) != 0)
		return 0;

	sp = _syspopup_find_by_id(id);
	if (sp == NULL)
		return 0;

	dpy = XOpenDisplay(NULL);

	_D("find key down: %s", sp->name);
	if (sp->endkey_act == SYSPOPUP_KEYEND_TERM) {
		if (sp->def_term_fn)
			sp->def_term_fn(sp->dupped_bundle,
					sp->user_data);

		win = (Window)sp->internal_data;
		XKillClient(dpy, win);
	} else if (sp->endkey_act == SYSPOPUP_KEYEND_HIDE) {
		if (sp->def_term_fn)
			sp->def_term_fn(sp->dupped_bundle,
					sp->user_data);

		win = (Window)sp->internal_data;
		XUnmapWindow(dpy, win);
	} else {
		_E("no find key down");
	}

	XCloseDisplay(dpy);

	return 0;
}
