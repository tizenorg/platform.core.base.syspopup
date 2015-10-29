/*
 * syspopup x11
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

#ifndef __SYSPOPUP_WAYLAND_H__
#define __SYSPOPUP_WAYLAND_H__

#include <bundle.h>

#include "syspopup_core.h"
#include "syspopup_db.h"

int wl_syspopup_init(syspopup *sp, syspopup_info_t *info);
int wl_syspopup_reset(bundle *b);
int wl_syspopup_process_keypress(int id, const char *keyname);

#endif /* __SYSPOPUP_WAYLAND_H__ */
