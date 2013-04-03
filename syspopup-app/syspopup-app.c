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
#include <appcore-efl.h>
#include <Ecore_X.h>

#include "syspopup.h"
#include "syspopup-app.h"

#include <time.h>

#include <dlog.h>

#undef LOG_TAG
#define LOG_TAG "SYSPOPUP-APP"

#define _E(fmt, arg...) LOGE(fmt,##arg)
#define _D(fmt, arg...) LOGD(fmt,##arg)

/*
struct quickpanel {
	int type;
	int priv_id; 
	char *title;
	char *content;
	time_t rtime;
	bundle *args; 
	bundle *args_group;
	int group_id;
};
*/

int myterm(bundle *b, void *data)
{
	/*
	char b_str[QP_BUF_LEN];
	int ret = 0;
	const char* val;

	struct appdata *ad = data;
	struct quickpanel* qp;

	val = bundle_get_val(b,"_INTERNAL_SYSPOPUP_NAME_");
	printf("[SYSTEM POPUP] Popup Name : %s\n", val);

	qp = malloc(sizeof(struct quickpanel));
	memset(qp,0,sizeof(struct quickpanel));

	//qp->type = QP_TYPE_SYSPOPUP;
	qp->type = quickpanel_get_type_id("org.tizen.syspopup-app", NULL, 0);
	qp->priv_id = QP_PRIVID;
	qp->title = ad->title;
	qp->content = ad->content;
	qp->args = b;
	qp->group_id = qp->priv_id;

	ret = quickpanel_insert(qp);
	if (ret < 0)
		printf("[SYSTEM POPUP] Error!! Cannot insert to quickpanel!\n");

	free(qp);
	*/
	return 0;
}

int mytimeout(bundle *b, void* data)
{
	/*
	char b_str[QP_BUF_LEN];
	int ret = 0;
	const char* val;

	struct appdata *ad = data;
	struct quickpanel* qp;

	val = bundle_get_val(b,"_INTERNAL_SYSPOPUP_NAME_");
	printf("[SYSTEM POPUP] Popup Name : %s\n", val);

	qp = malloc(sizeof(struct quickpanel));
	memset(qp,0,sizeof(struct quickpanel));

	//qp->type = QP_TYPE_SYSPOPUP;
	qp->type = quickpanel_get_type_id("org.tizen.syspopup-app", NULL, 0);
	qp->priv_id = QP_PRIVID;
	qp->title = ad->title;
	qp->content = ad->content;
	qp->args = b;
	qp->group_id = qp->priv_id;

	ret = quickpanel_insert(qp);
	if (ret < 0)
		printf("[SYSTEM POPUP] Error!! Cannot insert to quickpanel!\n");

	free(qp);
	*/
	return 0;
}

syspopup_handler handler = {
	.def_term_fn = myterm,
	.def_timeout_fn = mytimeout
};

static void __win_del(void *data, Evas_Object * obj, void *event)
{
	elm_exit();
}

/*
static int rotate(enum appcore_rm m, void *data)
{
	struct appdata *ad = data;
	int r;

	if (ad == NULL || ad->win == NULL)
		return 0;

	switch(m) {
	case APPCORE_RM_PORTRAIT_NORMAL:
		r = 0;
		break;
	case APPCORE_RM_PORTRAIT_REVERSE:
		r = 180;
		break;
	case APPCORE_RM_LANDSCAPE_NORMAL:
		r = 270;
		break;
	case APPCORE_RM_LANDSCAPE_REVERSE:
		r = 90;
		break;
	default:
		r = -1;
		break;
	}

	if (r >= 0)
		elm_win_rotation_with_resize_set(ad->win, r);

	return 0;
}
*/

static Evas_Object *__create_win(const char *name)
{
	Evas_Object *eo;
	int w;
	int h;

	eo = elm_win_add(NULL, name, ELM_WIN_DIALOG_BASIC);
	if (eo) {
		elm_win_title_set(eo, name);
		elm_win_borderless_set(eo, EINA_TRUE);

		elm_win_alpha_set(eo, EINA_TRUE);

		evas_object_smart_callback_add(eo, "delete,request",
					       __win_del, NULL);
		ecore_x_window_size_get(ecore_x_window_root_first_get(),
					&w, &h);
		evas_object_resize(eo, w, h);
	}

	return eo;
}

static int __app_create(void *data)
{
	struct appdata *ad = data;
	Evas_Object *win;
	int r;

	/* create window */
	win = __create_win(PACKAGE);
	if (win == NULL)
		return -1;
	ad->win = win;
	/* evas_object_show(win);*/

	/* init internationalization */
	r = appcore_set_i18n(PACKAGE, LOCALEDIR);
	if (r)
		return -1;

	/* appcore_set_rotation_cb(rotate, ad);*/

	appcore_measure_start();
	return 0;
}

static int __app_terminate(void *data)
{
	struct appdata *ad = data;

	_D("__app_terminate called");

	if (ad->win)
		evas_object_del(ad->win);

	return 0;
}

static int __app_pause(void *data)
{
	struct appdata *ad = data;

	return 0;
}

static int __app_resume(void *data)
{
	struct appdata *ad = data;

	return 0;
}

static void __prt_recvd_bundle(const char *key, const char *value, void *d)
{
	_D("recvd - key: %s, value: %s\n", key, value);
}

static void __response_cb(void *data, Evas_Object * obj, void *event_info)
{
	if ((int)event_info != 5)
		evas_object_del(obj);
	elm_exit();
}

static void _block_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	evas_object_del(obj);
}

static int __app_reset(bundle *b, void *data)
{
	struct appdata *ad = data;
	Evas_Object *popup;
	const char *val;
	int ret = 0;

	ad->b = bundle_dup(b);

	bundle_iterate(b, __prt_recvd_bundle, NULL);

	if (syspopup_has_popup(b)) {
		syspopup_reset(b);
	} else {
		popup = elm_popup_add(ad->win);
		if (popup != NULL) {
			ret = syspopup_create(b, &handler, ad->win, ad);
			evas_object_show(ad->win);

			if (ret == 0) {
				val = bundle_get_val(b, "_SYSPOPUP_TITLE_");
				if (val) {
					snprintf(ad->title, TITLE_BUF_LEN, "%s",
						 val);
				} else {
					snprintf(ad->title, TITLE_BUF_LEN, "%s",
						 "Unknown Title");
				}

				val = bundle_get_val(b, "_SYSPOPUP_CONTENT_");
				if (val) {
					snprintf(ad->content, CONTENT_BUF_LEN,
						 "%s", val);
				} else {
					snprintf(ad->content, CONTENT_BUF_LEN,
						 "%s", "Unknown Content");
				}

				elm_object_style_set(popup, "char_wrap_style");
				evas_object_size_hint_weight_set(popup,
							EVAS_HINT_EXPAND,
							EVAS_HINT_EXPAND);
				evas_object_smart_callback_add(popup, "block,clicked", _block_clicked_cb, NULL);
				elm_object_part_text_set(popup, "title,text", ad->title);
				elm_object_text_set(popup, ad->content);
				evas_object_smart_callback_add(popup,
							       "response",
							       __response_cb,
							       NULL);

				evas_object_show(popup);
			}
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	struct appdata ad;
	struct appcore_ops ops = {
		.create = __app_create,
		.terminate = __app_terminate,
		.pause = __app_pause,
		.resume = __app_resume,
		.reset = __app_reset,
	};

	memset(&ad, 0x0, sizeof(struct appdata));
	ops.data = &ad;

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}

