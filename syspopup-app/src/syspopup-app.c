/*
 * syspopup-app
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


#include <stdio.h>
#include <app.h>
#include <app_control.h>
#include <app_control_internal.h>
#include <Elementary.h>
#include <system_settings.h>
#include <bundle_internal.h>
#include <efl_extension.h>

#include "syspopup.h"
#include "syspopup-app.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *popup;
} appdata_s;

syspopup_handler syspopup_h = {
	.def_term_fn = NULL,
	.def_timeout_fn = NULL,
};

static void win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static Evas_Object *create_win(const char *name)
{
	Evas_Object *win;

	win = elm_win_add(NULL, name, ELM_WIN_DIALOG_BASIC);
	if (win == NULL)
		return NULL;

	elm_win_title_set(win, name);
	elm_win_borderless_set(win, EINA_TRUE);
	elm_win_alpha_set(win, EINA_TRUE);

	evas_object_smart_callback_add(win, "delete,request",
					win_delete_request_cb, NULL);

	return win;
}

static bool app_create(void *data)
{
	appdata_s *ad = data;

	ad->win = create_win(PACKAGE);
	if (ad->win == NULL)
		return false;

	return true;
}

static void __exit_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void set_popup_text(Evas_Object *popup, bundle *b)
{
	const char *value;
	const char *title = "";
	const char *content = "";

	if (popup == NULL)
		return;

	value = bundle_get_val(b, KEY_SYSPOPUP_TITLE);
	if (value)
		title = value;

	elm_object_part_text_set(popup, "title,text", title);

	value = bundle_get_val(b, KEY_SYSPOPUP_CONTENT);
	if (value)
		content = value;

	elm_object_text_set(popup, content);
}

static void create_popup(appdata_s *ad, bundle *b)
{
	int ret;

	ad->popup = elm_popup_add(ad->win);
	if (ad->popup == NULL)
		return;

	ret = syspopup_create(b, &syspopup_h, ad->win, ad);
	if (ret < 0)
		return;

	elm_object_style_set(ad->popup, "char_wrap_style");
	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(ad->popup, "block,clicked", __exit_cb, ad);
	evas_object_smart_callback_add(ad->popup, "response", __exit_cb, ad);
	evas_object_smart_callback_add(ad->popup, EEXT_CALLBACK_BACK, __exit_cb, ad);
	set_popup_text(ad->popup, b);
	evas_object_show(ad->popup);
	evas_object_show(ad->win);
}

static void app_control(app_control_h app_control, void *data)
{
	int ret;
	bundle *b = NULL;
	appdata_s *ad = data;

	ret = app_control_to_bundle(app_control, &b);
	if (ret != APP_CONTROL_ERROR_NONE)
		return;

	if (syspopup_has_popup(b)) {
		syspopup_reset(b);
		return;
	}

	create_popup(ad, b);
}

static void app_terminate(void *data)
{
	appdata_s *ad = data;

	if (ad == NULL)
		return;

	if (ad->popup)
		evas_object_del(ad->popup);
	if (ad->win)
		evas_object_del(ad->win);
}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);

	return;
}

int main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;
	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers;

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers, APP_EVENT_LANGUAGE_CHANGED,
					ui_app_lang_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);

	return ret;
}
