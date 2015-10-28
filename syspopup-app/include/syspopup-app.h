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

#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "SYSPOPUP-APP"

#ifndef _ERR
#define _ERR(fmt, args...) LOGE("[%s:%d] "fmt"\n", __func__, __LINE__, ##args)
#endif

#ifndef _DBG
#define _DBG(fmt, args...) LOGD("[%s:%d] "fmt"\n", __func__, __LINE__, ##args)
#endif

#ifndef _INFO
#define _INFO(fmt, args...) LOGI("[%s:%d] "fmt"\n", __func__, __LINE__, ##args)
#endif

#ifndef _E
#define _E _ERR
#endif

#ifndef _D
#define _D _DBG
#endif

#ifndef _I
#define _I _INFO
#endif

#ifndef PACKAGE
#define PACKAGE "syspopup-app"
#endif

#define KEY_SYSPOPUP_TITLE "_SYSPOPUP_TITLE_"
#define KEY_SYSPOPUP_CONTENT "_SYSPOPUP_CONTENT_"

#define BUF_LEN 256

#endif /* __SYSPOPUP_APP_H__ */
