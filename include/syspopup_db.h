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


#ifndef __SYSPOPUP_DB_H__
#define __SYSPOPUP_DB_H__

enum _def_term_action_t {
	SYSPOPUP_TERM,
	SYSPOPUP_HIDE,
	SYSPOPUP_IGNORE
};

typedef enum _def_term_action_t def_term_action_t;

enum _def_endkey_action_t {
	SYSPOPUP_KEYEND_TERM,
	SYSPOPUP_KEYEND_HIDE,
	SYSPOPUP_KEYEND_IGNORE
};

typedef enum _def_endkey_action_t def_endkey_action_t;

struct _syspopup_info_t {
	char *name;		/* system popup name */
	int prio;		/* sys popup priority :
					 cf - Utilx_Notification_Level */
	int focus;		/* sys popup focus : ecore_x_icccm_hints_set */
	unsigned int timeout;	/* default system popup timeout */
	def_term_action_t term_act;	/* default terminate act */
	def_endkey_action_t endkey_act;	/* default terminate act */
	char *pkgname;		/* package name to be launched */
};

typedef struct _syspopup_info_t syspopup_info_t;

int _syspopup_info_add(syspopup_info_t *pinfo);
syspopup_info_t *_syspopup_info_get(const char *popup_name);
void _syspopup_info_free(syspopup_info_t *pinfo);

#endif
