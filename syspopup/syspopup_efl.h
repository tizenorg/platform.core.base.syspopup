/*
 * syspopup
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd. All rights reserved.
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

#ifndef __SYSPOPUP_EFL_H__
#define __SYSPOPUP_EFL_H__

#include <bundle.h>

#include "syspopup.h"

int syspopup_efl_create(const char *popup_name, bundle *b, Evas_Object *parent,
		syspopup_handler *handler, void *user_data);
int syspopup_efl_reset(bundle *b);

#endif /* __SYSPOPUP_EFL_H__ */
