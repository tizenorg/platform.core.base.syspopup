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

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Input.h>

#include "syspopup_core.h"
#include "syspopup.h"
#include "syspopup_api.h"
#include "simple_util.h"
#if defined(X11)
#include "syspopup_x.h"
#elif defined(WAYLAND)
#include "syspopup_wayland.h"
#endif

static Eina_Bool __keydown_cb(void *data, int type, void *event)
{
	int id = (int)data;
	Ecore_Event_Key *ev = event;

	if (ev == NULL)
		return ECORE_CALLBACK_DONE;

#if defined(X11)
	x_syspopup_process_keypress(id, ev->keyname);
#elif defined(WAYLAND)
	wl_syspopup_process_keypress(id, ev->keyname);
#endif

	return ECORE_CALLBACK_DONE;
}

static int __efl_rotate_cb(void *d, void *w, void *s)
{
	return 0;
}

int syspopup_efl_create(const char *popup_name, bundle *b, Evas_Object *parent,
			syspopup_handler *handler, void *user_data)
{
	syspopup *sp;
	syspopup_info_t *info;

	info = _syspopup_info_get(popup_name);
	if (info == NULL) {
		_E("Failed to get syspopup info: %s", popup_name);
		return -1;
	}

	sp = (syspopup *)malloc(sizeof(syspopup));
	if (sp == NULL) {
		_syspopup_info_free(info);
		return -1;
	}

	sp->name = strdup(info->name);
	sp->def_term_fn = handler->def_term_fn;
	sp->def_timeout_fn = handler->def_timeout_fn;
	sp->user_data = user_data;
	sp->win = (void *)parent;
	sp->timeout_id = 0;
	sp->dupped_bundle = bundle_dup(b);
	sp->rotate_cb = __efl_rotate_cb;

	_syspopup_add_new(sp);
	_syspopup_set_term_type(sp, info);
	_syspopup_set_endkey_type(sp, info);

#if defined(X11)
	sp->internal_data = (void *)elm_win_xwindow_get(parent);
	if (x_syspopup_init(sp, info) < 0) {
		_syspopup_info_free(info);
		_syspopup_del(sp->id);
		return -1;
	}
#elif defined(WAYLAND)
	sp->internal_data = (void *)elm_win_wl_window_get(parent);
	if (wl_syspopup_init(sp, info) < 0) {
		_syspopup_info_free(info);
		_syspopup_del(sp->id);
		return -1;
	}
#endif

	_syspopup_reset_timeout(sp, info);
	ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
				__keydown_cb, (const void *)sp->id);

	_syspopup_info_free(info);

	return sp->id;
}

int syspopup_efl_reset(bundle *b)
{
#if defined(X11)
	return x_syspopup_reset(b);
#elif defined(WAYLAND)
	return wl_syspopup_reset(b);
#endif
}
