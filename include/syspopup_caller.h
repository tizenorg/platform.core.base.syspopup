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


#ifndef __SYSPOPUP_CALLER_H__
#define __SYSPOPUP_CALLER_H__

#include <bundle.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file        syspopup_caller.h
 * @brief       Library to launch/destroy System Popup Application
 *
 * Patched by   Knhoon Baik <knhoon.baik@samsung.com>
 * Patched by   Noha Park <noha.park@samsung.com>
 */

/**
 * @defgroup syspopup_caller    Helper Library to launch/destroy system popup application
 * @ingroup syspopup
 * @brief                       Helper Library to launch/destroy system popup application
 */

/**
 * @addtogroup syspopup_caller
 * @{
 */

/**
 * @brief       This API launch the system popup application with given popup name.
 *
 *              This API launch the system popup application.
 *              This API find system popup application package name with given popup name from system popup infomation DB.\n
 *              And then, launch found application package
 *
 * @param[in]   popup_name      system popup application name to launch (not package name)
 * @param[in]   b               extra bundle(arguement) to toss the popup application
 * @return      0 if success, negative value(<0) if fail
 * @retval      0               - success
 * @retval      -1              - generic error
 *
 * @sample code
 * @code
 *
 * ...
 *	bundle *b = NULL;
 *
 *	b = bundle_create();
 *	bundle_add(b, "_SYSPOPUP_TITLE_", "System Popup Title");
 *	bundle_add(b, "_SYSPOPUP_CONTENT_", "System Popup Content");
 *
 *	ret = syspopup_launch("syspopup-app", b);
 *
 *	bundle_free(b);
 * ...
 *
 * @endcode
 */
int syspopup_launch(char *popup_name, bundle *b);

/**
 * @brief       This API generate destroy event to all system popup application.
 *		This API can call with proper permission (root or inhouse)
 *
 * @return      0 if success, negative value(<0) if fail
 * @retval      0               - success
 * @retval      -1              - generic error
 */
int syspopup_destroy_all(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
