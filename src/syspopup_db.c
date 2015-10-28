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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

/* For multi-user support */
#include <tzplatform_config.h>

#include "syspopup_db.h"
#include "simple_util.h"

#define SYSPOPUP_DB_PATH tzplatform_mkpath(TZ_SYS_DB, ".syspopup.db")
#define QUERY_MAXLEN 4096

#define SP_INFO_TBL "syspopup_info"
#define SP_INFO_TBL_F_NAME "name"

static sqlite3 *db = NULL;

/* db initialize */
static int __init(void)
{
	int rc;

	if (db) {
		_D("Already initialized\n");
		return 0;
	}

	rc = sqlite3_open(SYSPOPUP_DB_PATH, &db);
	if (rc != SQLITE_OK) {
		_E("Can't open database: %s / %d / %d", sqlite3_errmsg(db),
				rc, sqlite3_extended_errcode(db));
		return -1;
	} else {
		_D("db open success");
	}

	return 0;
}

static int __fini(void)
{
	if (db) {
		sqlite3_close(db);
		db = NULL;
	}

	return 0;
}

syspopup_info_t *_syspopup_info_get(const char *popup_name)
{
	int rc;
	char sqlbuf[256] = {0,};
	sqlite3_stmt *stmt = NULL;
	syspopup_info_t *pinfo = NULL;

	if (popup_name == NULL) {
		_E("no popup name");
		return NULL;
	}

	if (__init() < 0)
		return NULL;

	snprintf(sqlbuf, sizeof(sqlbuf),
		"SELECT name,prio,focus,timeout,term_act,endkey_act,pkgname FROM %s WHERE %s = ?;",
		SP_INFO_TBL, SP_INFO_TBL_F_NAME);

	rc = sqlite3_prepare_v2(db, sqlbuf, -1, &stmt, NULL);
	if (rc != SQLITE_OK) {
		_E("popup info prepare error(%d)", rc);
		goto out;
	}

	rc = sqlite3_bind_text(stmt, 1, popup_name, strlen(popup_name), SQLITE_STATIC);
	if (rc != SQLITE_OK) {
		_E("popup name bind error(%d)", rc);
		goto out;
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW) {
		_E("error(%d) in prepare", rc);
		goto out;
	}

	pinfo = (syspopup_info_t *)malloc(sizeof(syspopup_info_t));
	if (pinfo == NULL) {
		_E("malloc error");
		goto out;
	}

	pinfo->name = strdup((char *)sqlite3_column_text(stmt, 0));
	pinfo->prio = sqlite3_column_int(stmt, 1);
	pinfo->focus = sqlite3_column_int(stmt, 2);
	pinfo->timeout = sqlite3_column_int(stmt, 3);
	pinfo->term_act = sqlite3_column_int(stmt, 4);
	pinfo->endkey_act = sqlite3_column_int(stmt, 5);

	if (sqlite3_column_text(stmt, 6) != NULL)
		pinfo->pkgname = strdup((char *) sqlite3_column_text(stmt, 6));
	else
		pinfo->pkgname = NULL;

out:
	sqlite3_finalize(stmt);
	__fini();

	return pinfo;
}

void _syspopup_info_free(syspopup_info_t *pinfo)
{
	if (pinfo->name)
		free(pinfo->name);

	if (pinfo->pkgname)
		free(pinfo->pkgname);

	free(pinfo);
}
