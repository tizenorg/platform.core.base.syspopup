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


#ifndef __SIMPLE_UTIL__
#define __SIMPLE_UTIL__

#include <unistd.h>
#include <ctype.h>
#include <dlog.h>

#undef LOG_TAG
#define LOG_TAG "SYSPOPUP"

#define MAX_LOCAL_BUFSZ 128

#define _E(fmt, arg...) LOGE(fmt,##arg)
#define _D(fmt, arg...) LOGD(fmt,##arg)
#define _I(fmt, arg...) LOGI(fmt,##arg)

#define SYSPOPUP_DBUS_PATH "/syspopup/dbus_handler"
#define SYSPOPUP_DBUS_SIGNAL_INTERFACE "org.tizen.syspopup.signal"
#define SYSPOPUP_DBUS_SP_TERM_SIGNAL	"syspopup_term"

#define AUL_SP_DBUS_PATH "/Org/Tizen/Aul/Syspopup"
#define AUL_SP_DBUS_SIGNAL_INTERFACE "org.tizen.aul.syspopup"
#define AUL_SP_DBUS_LAUNCH_REQUEST_SIGNAL "syspopup_launch_request"

#endif

