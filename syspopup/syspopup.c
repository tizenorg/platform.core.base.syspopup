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

#ifdef HAVE_X
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

#ifdef HAVE_X
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
		for (i = 0; i < len; i++) {
			val[i] = ((unsigned long *)prop_ret)[i];
		}
		num = len;
	}

	if (prop_ret)
		XFree(prop_ret);

	return num;
}
#endif

/*
static Window get_active_win(Display *dpy, Window win, Atom property)
{
    Window active_win = None;
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop_return = NULL;

    if(Success == XGetWindowProperty(dpy, win, property, 0L, sizeof(Window),
				     False, XA_WINDOW, &actual_type,
				     &actual_format, &nitems, &bytes_after,
				     &prop_return) && prop_return) {
	active_win = *(Window *)prop_return;
	XFree(prop_return);
    }

    return active_win;
}
*/

static void __X_syspopup_term_handler(void *data)
{
#ifdef HAVE_X
	syspopup *tmp;
	Display *d;
	Window win;

	_D("enter syspopup term handler");

	d = XOpenDisplay(NULL);
	tmp = _syspopup_get_head();
	while (tmp) {
		switch (tmp->term_act) {
		case SYSPOPUP_TERM:
			if (tmp->def_term_fn != NULL)
				tmp->def_term_fn(tmp->dupped_bundle,
						 tmp->user_data);

			win = (Window) tmp->internal_data;
			XKillClient(d, win);
			/*XDestroyWindow(d, win);*/ 
			/* TODO :modify for multi popup */
			break;
		case SYSPOPUP_HIDE:
			if (tmp->def_term_fn != NULL)
				tmp->def_term_fn(tmp->dupped_bundle,
						 tmp->user_data);

			win = (Window) tmp->internal_data;
			XUnmapWindow(d, win);
			break;
		default:
			_D("term action IGNORED - %s", tmp->name);
		}
		tmp = tmp->next;
	}

	XCloseDisplay(d);
	/*TODO : if there is no popup window, kill client*/
#endif
}

static gboolean __X_syspopup_timeout_handler(void *user_data)
{
#ifdef HAVE_X
	syspopup *sp = NULL;
	Display *d;
	int id;
	Window win;

	id = (int)user_data;
	d = XOpenDisplay(NULL);

	sp = _syspopup_find_by_id(id);
	if (sp != NULL) {
		_D("find timeout - %s", sp->name);
		if (sp->def_timeout_fn != NULL)
			sp->def_timeout_fn(sp->dupped_bundle, sp->user_data);
		win = (Window) sp->internal_data;
		XKillClient(d, win);
	} else {
		_E("no find timeout");
	}

	XCloseDisplay(d);
#endif

	return 0;
}

#ifdef HAVE_X
static int __X_syspopup_change_xwin_type(Display *dpy, Window win)
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
#endif

#ifdef HAVE_X
static int __X_syspopup_disable_focus(Display *dpy, Window win)
{
	XWMHints *hints;

	hints = XAllocWMHints();
	if (!hints) return -1;

	hints->flags = InputHint | StateHint;
	hints->input = 0;
	hints->initial_state = NormalState;

	XSetWMHints(dpy, win, hints);
	XFree(hints);

	return 0;
}
#endif

#ifdef HAVE_X
int X_syspopup_rotation_get(Display *dpy, Window win)
{
	Window active_win;
	Window root_win;
	int rotation = -1;
	int ret;

	int angles[2];

	Atom atom_active_win;
	Atom atom_win_rotate_angle;

	root_win = XDefaultRootWindow(dpy);

	atom_active_win = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
	ret = __utilx_ss_get_window_property(dpy, root_win, atom_active_win,
					     XA_WINDOW,
					     (unsigned int *)&active_win, 1);

	/*printf("[SYSPOPUP] Active win : %x, Window %x\n", active_win, win);*/

	/*active_win = get_active_win(dpy, root_win, atom_active_win);*/
	if (ret < 0)
		return ret;

	atom_win_rotate_angle =
		XInternAtom(dpy, "_E_ILLUME_ROTATE_ROOT_ANGLE", False);
	ret = __utilx_ss_get_window_property(dpy, root_win,
					  atom_win_rotate_angle, XA_CARDINAL,
					  (unsigned int *)&rotation, 1);

	/*printf("[SYSPOPUP] Rotation %d\n", rotation);*/

	if (ret != -1)
		return rotation;

	return -1;
}
#endif

int X_syspopup_process_keydown(int id, const char *keyname)
{
#ifdef HAVE_X
	Display *d;
	Window win;
	syspopup *sp = NULL;

	if (strcmp(keyname, KEY_END) == 0) {
		d = XOpenDisplay(NULL);
		sp = _syspopup_find_by_id(id);
		if (sp != NULL) {
			_D("find key down - %s", sp->name);
			if (sp->endkey_act == SYSPOPUP_KEYEND_TERM) {
				if (sp->def_term_fn != NULL)
					sp->def_term_fn(sp->dupped_bundle,
							sp->user_data);
				win = (Window) sp->internal_data;
				XKillClient(d, win);

			} else if (sp->endkey_act == SYSPOPUP_KEYEND_HIDE) {
				if (sp->def_term_fn != NULL)
					sp->def_term_fn(sp->dupped_bundle,
							sp->user_data);
				win = (Window) sp->internal_data;
				XUnmapWindow(d, win);
			}

		} else {
			_E("no find key down");
		}

		XCloseDisplay(d);
	}
#endif

	return 0;
}

int X_syspopup_process_rotate(int id)
{
#ifdef HAVE_X
	Display *d;
	Window win;
	syspopup *sp = NULL;

	sp = _syspopup_find_by_id(id);

	if (sp == NULL)
		return -1;

	win = (Window) sp->internal_data;

	d = XOpenDisplay(NULL);
	sp->rotate_cb(d, win, sp);
	XCloseDisplay(d);
#endif

	return 0;
}

#ifdef HAVE_X
int X_make_syspopup(bundle *b, Display *dpy, Window xwin, void *win,
		    int (*rotate_func) (Display*, Window, syspopup*),
		    syspopup_handler *handler, void *user_data)
{
	syspopup *sp = NULL;
	syspopup_info_t *info;
	const char *popup_name = _syspopup_get_name_from_bundle(b);
	XWindowAttributes attr;
	int is_unviewable = 0;

	if (popup_name == NULL || handler == NULL)
		return -1;

	info = _syspopup_info_get(popup_name);
	if (info == NULL)
		return -1;

	if (_syspopup_init(__X_syspopup_term_handler,
			   __X_syspopup_timeout_handler) < 0){
		_syspopup_info_free(info);
		return -1;
	}

	XGetWindowAttributes(dpy, xwin, &attr);

	if (attr.map_state == IsViewable) {
		XUnmapWindow(dpy, xwin);
		is_unviewable = 1;
	}

	sp = (syspopup *) malloc(sizeof(syspopup));
	if (sp == NULL) {
		_syspopup_info_free(info);
		return -1;
	}
	
	sp->name = strdup(info->name);
	sp->def_term_fn = handler->def_term_fn;
	sp->def_timeout_fn = handler->def_timeout_fn;
	sp->user_data = user_data;
	sp->internal_data = (void *)xwin;
	sp->win = (void *)win;
	sp->rotate_cb = rotate_func;
	sp->timeout_id = 0;
	sp->dupped_bundle = bundle_dup(b);
	_syspopup_add_new(sp);

	_syspopup_set_term_type(sp, info);
	_syspopup_set_endkey_type(sp, info);

	_syspopup_reset_timeout(sp, info);

	__X_syspopup_change_xwin_type(dpy, xwin);
	utilx_set_system_notification_level(dpy, xwin, info->prio);

	utilx_grab_key(dpy, xwin, KEY_END, TOP_POSITION_GRAB);

	if (info->focus == 1) {
		__X_syspopup_disable_focus (dpy, xwin);
	}

	rotate_func(dpy, xwin, sp);

	if (is_unviewable == 1) {
		XMapWindow(dpy, xwin);
	}

	_syspopup_info_free(info);

	return sp->id;
}
#endif

/**
 * @brief       This API reset created the system popup's properties
 *
 *		This API reset created the system popup's properties based on
 *		system popup information DB after extracting popup name from 
 *		given bundle system popup properties to be reset : timeout, 
 *		default action type, ....
 *
 * @param[in]   b		bundle received by app_reset handler
 *				(included system popup name)
 * @return      0 if success, negative value(<0) if fail
 * @retval      0	        - success
 * @retval      -1	        - generic error
 */
int X_syspopup_reset(bundle *b)
{
#ifdef HAVE_X
	const char *popup_name;
	syspopup_info_t *info;
	syspopup *sp = NULL;
	int (*rotate_func) (Display *, Window, syspopup *);

	popup_name = _syspopup_get_name_from_bundle(b);
	if (popup_name == NULL)
		return -1;

	sp = _syspopup_find(popup_name);
	if (!sp)
		return -1;
	else {
		Display *d;
		Window win;

		info = _syspopup_info_get(popup_name);
		if (info == NULL)
			return -1;
		_syspopup_reset_timeout(sp, info);

		if (sp->dupped_bundle)
			free(sp->dupped_bundle);
		sp->dupped_bundle = bundle_dup(b);

		d = XOpenDisplay(NULL);
		win = (Window) sp->internal_data;
		utilx_set_system_notification_level(d, win, info->prio);

		if (info->focus == 1) {
			__X_syspopup_disable_focus (d, win);
		}
		rotate_func = sp->rotate_cb;
		rotate_func(d, win, sp);

		XMapWindow(d, win);
		/*XMapRaised(d,win);*/
		XCloseDisplay(d);

		_syspopup_info_free(info);
	}
#endif

	return 0;
}

API int syspopup_has_popup(bundle *b)
{
	const char *popup_name;
	popup_name = _syspopup_get_name_from_bundle(b);
	if (popup_name == NULL)
		return 0;

	if (_syspopup_find(popup_name) != NULL)
		return 1;
	else
		return 0;
}

API int syspopup_reset_timeout(bundle *b, unsigned int time)
{
	const char *popup_name;
	syspopup_info_t *info;
	syspopup *sp = NULL;
	int ret;

	popup_name = _syspopup_get_name_from_bundle(b);
	if (popup_name == NULL) {
		_E("popup_name is null");
		return -1;
	}

	sp = _syspopup_find(popup_name);
	if (!sp) {
		_E("find syspopup error");
		return -1;
	} else {
		info = _syspopup_info_get(popup_name);
		if (info == NULL) {
			_E("get syspopup info error");
			return -1;
		}
		info->timeout = time;
		ret = _syspopup_reset_timeout(sp, info);
		_syspopup_info_free(info);
	}

	return ret;
}

