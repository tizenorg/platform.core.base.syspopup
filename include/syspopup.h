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


#ifndef __SYSPOPUP_H__
#define __SYSPOPUP_H__

#include <utilX.h>
#include <bundle.h>
#include <Elementary.h>

/**
 * @mainpage
 * 
 * This is System Popup Library 
 * This library help to create system popup application & to launch/destroy system popup application
 *
 */

/**
 * @close
 * @ingroup APPLICATION_FRAMEWORK
 * @defgroup syspopup System Popup Library 
 * @{
 */

/**
 * @file        syspopup.h 
 * @brief       System Popup UI Library
 *
 * Patched by   Knhoon Baik <knhoon.baik@samsung.com>
 * Patched by	Noha Park <noha.park@samsung.com> 
 */

/**
 * @defgroup syspopup_ui	Helper Library to create system popup application
 * @ingroup syspopup 
 * @brief			Helper Library to create system popup application
 */

/**
 * @addtogroup syspopup_ui
 * @{
 */

/** 
 *@brief action handler supplied by user 
 *	def_term_fn is called when event to destroy all system popup occur
 *	def_timeout_fn is called when the system popup's timeout is expired
 */
struct _syspopup_handler {
	int (*def_term_fn) (bundle *, void *);
	/**< handler supplied by user to process termination request */
	int (*def_timeout_fn) (bundle *, void *);
	/**< handler supplied by user to process timeout */
};

typedef struct _syspopup_handler syspopup_handler;

/**
 * @brief       This API make elm system popup window
 *
 *              This API make elm system popup window. 
 *		and then, make given elm system popup window as system popup type
 *		and set system popup properties based on system popup information DB.\n
 *		system popup properties to be set : timeout, default action type, ....
 *
 * @param[in]   b		bundle received by app_reset handler (included system popup name)
 * @param[in]   handler         user-supplied handler for handling system popup internal event.
 * @param[in]	parent		parent window created with elm_win_add.
 * @param[in]   user_data	user-supplied data
 * @return      0 if success, -1 if fail
 * @retval      0	        - success
 * @retval      -1		- generic error
 */
int syspopup_create(bundle *b, syspopup_handler *handler,
		    Evas_Object *parent, void *user_data);

/**
 * @brief       This API reset created the elm system popup's properties
 *
 * 		This API reset created the elm system popup's properties based on system popup information DB after extracting popup name from given bundle
 *		system popup properties to be reset : timeout, default action type, ....
 *
 * @param[in]   b		bundle received by app_reset handler (included system popup name)
 * @return      0 if success, negative value(<0) if fail
 * @retval      0	        - success
 * @retval      -1	        - generic error
 */
int syspopup_reset(bundle *b);

/**
 * @brief       This API ask that the system popup is created and is running 
 *
 *		This API ask that the the system popup with popup name is created and is running after extracting popup name from given bundle. 
 *
 * @param[in]   b		bundle received by app_reset handler (included system popup name)
 * @return	true / false      
 * @retval	1	app already has the system popup
 * @retval      0	app has not the system popup
 */
int syspopup_has_popup(bundle *b);

/**
 * @brief       This API reset timeout value of system popup.
 *
 *		This API reset timeout value of system popup.
 *
 * @param[in]   b	bundle received by app_reset handler (included system popup name)
 * @param[in]   time	timeout time (sec)
 * @return	0	if success, negative value(<0) if fail
 * @retval	0	success
 * @retval	-1	generic error
 */
int syspopup_reset_timeout(bundle *b, unsigned int time);

/** @} */

#endif

