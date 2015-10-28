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
#include "syspopup_efl.h"
#include "simple_util.h"

API int syspopup_create(bundle *b, syspopup_handler *handler,
			Evas_Object *parent, void *user_data)
{
	const char *popup_name;
	syspopup *sp;
	int id;

	if (parent == NULL || handler == NULL) {
		_E("parent window or handler is NULL");
		return -1;
	}

	popup_name = _syspopup_get_name_from_bundle(b);
	if (popup_name == NULL) {
		_E("popup_name is NULL");
		return -1;
	}

	sp = _syspopup_find(popup_name);
	if (sp) {
		_E("already exist - syspopup: %s", popup_name);
		return -1;
	}

	id = syspopup_efl_create(popup_name, b,	parent,
			handler, user_data);
	if (id < 0) {
		_E("Failed to create syspopup");
		return -1;
	}

	return 0;
}

API int syspopup_reset(bundle *b)
{
	return syspopup_efl_reset(b);
}

API int syspopup_has_popup(bundle *b)
{
	const char *popup_name;

	popup_name = _syspopup_get_name_from_bundle(b);
	if (popup_name == NULL)
		return 0;

	if (_syspopup_find(popup_name))
		return 1;

	return 0;
}

API int syspopup_reset_timeout(bundle *b, unsigned int time)
{
	const char *popup_name;
	syspopup_info_t *info;
	syspopup *sp;
	int ret;

	popup_name = _syspopup_get_name_from_bundle(b);
	if (popup_name == NULL) {
		_E("popup_name is null");
		return -1;
	}

	sp = _syspopup_find(popup_name);
	if (sp == NULL) {
		_E("find syspopup error");
		return -1;
	}

	info = _syspopup_info_get(popup_name);
	if (info == NULL) {
		_E("get syspopup info error");
		return -1;
	}

	info->timeout = time;
	ret = _syspopup_reset_timeout(sp, info);
	_syspopup_info_free(info);

	return ret;
}
