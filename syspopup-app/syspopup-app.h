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


#ifndef __SYSPOPUP_APP_H__
#define __SYSPOPUP_APP_H__

#include <Elementary.h>

#if !defined(PACKAGE)
#  define PACKAGE "syspopup-app"
#endif

#define NAME_BUF_LEN	256
#define TITLE_BUF_LEN	256
#define CONTENT_BUF_LEN	256

#define QP_BUF_LEN	10
#define QP_PRIVID 1
#define QP_TERM	1
#define QP_TIMEOUT	1

struct appdata {
	Evas_Object *win;
	Evas_Object *popup;
	bundle *b;
	char title[TITLE_BUF_LEN];
	char content[CONTENT_BUF_LEN];
	/* add more variables here */
};

#endif				/* __SYSPOPUP_APP_H__ */

