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


#ifndef __SYSPOPUP_CORE_H__
#define __SYSPOPUP_CORE_H__

#include <glib.h>
#include <bundle.h>
#include "syspopup_db.h"

#define SYSPOPUP_DEFAULT_TIMEOUT 20	/* sec */
#define SYSPOPUP_TERM_NOTI_PATH  "/usr/share/popup_noti_term"

struct _syspopup {
	int id;
	char *name;
	int (*def_term_fn) (bundle *, void *);
	int (*def_timeout_fn) (bundle *, void *);
	guint timeout_id;
	def_term_action_t term_act;
	def_endkey_action_t endkey_act;
	void *user_data;
	void *internal_data;
	void *win;
	int (*rotate_cb) (void *, void *, void *);
	bundle *dupped_bundle;
	struct _syspopup *next;
};

typedef struct _syspopup syspopup;

syspopup *_syspopup_get_head();
int _syspopup_add_new(syspopup *pinfo);
syspopup *_syspopup_find(const char *name);
syspopup *_syspopup_find_by_id(int id);
void _syspopup_del(int id);

int _syspopup_init(void (*term_handler) (void *),
		   gboolean(*timeout_handler) (void *));
int _syspopup_reset_timeout(syspopup *sp, syspopup_info_t *info);
int _syspopup_set_term_type(syspopup *sp, syspopup_info_t *info);
int _syspopup_set_endkey_type(syspopup *sp, syspopup_info_t *info);

const char *_syspopup_get_name_from_bundle(bundle *b);
int _syspopup_set_name_to_bundle(bundle *b, char *popup_name);

#endif

