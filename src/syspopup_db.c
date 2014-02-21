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
#define QUERY_MAXLEN	4096

#define SP_INFO_TBL		"syspopup_info"
#define SP_INFO_TBL_F_NAME	"name"

static sqlite3 *db = NULL;

/**
 * exec  
 * param[in] db handler
 * param[in] query query
 * return This method returns 0 (SUCCESS) or -1 (FAIL)
 */
static int __exec(sqlite3 *db, char *query)
{
	int rc = 0;
	char *errmsg = NULL;

	if (db == NULL) {
		_E("DB handler is null");
		return -1;
	}
	rc = sqlite3_exec(db, query, NULL, 0, &errmsg);

	if (rc != SQLITE_OK) {
		_D("Query: [%s]", query);
		_E("SQL error: %s\n", errmsg);
		sqlite3_free(errmsg);
		return (-1);
	}

	return 0;
}

/**
 * db initialize
 */
static int __init(void)
{
	int rc;

	if (db) {
		_D("Already initialized\n");
		return 0;
	}

	rc = sqlite3_open(SYSPOPUP_DB_PATH, &db);
	if (rc) {
		_E("Can't open database: %s", sqlite3_errmsg(db));
		goto err;
	}
	/* Enable persist journal mode*/
	rc = sqlite3_exec(db, "PRAGMA journal_mode = PERSIST", NULL, NULL,
			  NULL);
	if (SQLITE_OK != rc) {
		_D("Fail to change journal mode\n");
		goto err;
	}

	return 0;
err:
	sqlite3_close(db);
	return -1;
}

static int __fini(void)
{
	if (db) {
		sqlite3_close(db);
		db = NULL;
	}
	return 0;
}

static int __delete_all(const char *tbl_name)
{
	char *_sqlbuf;
	int rc;

	_sqlbuf = sqlite3_mprintf("DELETE FROM %s;", tbl_name);
	rc = __exec(db, _sqlbuf);
	sqlite3_free(_sqlbuf);

	return rc;
}

static int __delete_with_field(const char *tbl_name, const char *f_name,
			       const char *val)
{
	char *_sqlbuf;
	int rc;

	_sqlbuf = sqlite3_mprintf("DELETE FROM %s WHERE %s = '%s';",
				  tbl_name, f_name, val);
	rc = __exec(db, _sqlbuf);
	sqlite3_free(_sqlbuf);

	return rc;
}

static int __count_with_field(const char *tbl_name, const char *f_name,
			      const char *val)
{
	char *_sqlbuf;
	int rc;
	char **db_result = NULL;
	char *db_err = NULL;
	int nrows = 0;
	int ncols = 0;
	int cnt;

	_sqlbuf = sqlite3_mprintf("SELECT COUNT(*) FROM %s WHERE %s = '%s';",
				  tbl_name, f_name, val);

	rc = sqlite3_get_table(db, _sqlbuf, &db_result, &nrows, &ncols,
			       &db_err);
	if (rc == -1 || nrows == 0) {
		_E("get count = 0 or fail");
		sqlite3_free_table(db_result);
		sqlite3_free(_sqlbuf);
		return 0;
	} else {
		cnt = atoi(db_result[1]);
		sqlite3_free_table(db_result);
		sqlite3_free(_sqlbuf);
	}

	return cnt;
}

int _syspopup_info_add(syspopup_info_t *pinfo)
{
	int rc = -1;
	int cnt = 0;
	char *_sqlbuf;

	if (pinfo->name == NULL) {
		_E("Name is null\n");
		return -1;
	}

	if (__init() < 0)
		return -1;

	cnt = __count_with_field(SP_INFO_TBL, SP_INFO_TBL_F_NAME, pinfo->name);

	if (cnt == 0) {
		_sqlbuf = sqlite3_mprintf("INSERT INTO %s "
					  "(name,prio,focus,timeout,term_act,endkey_act,pkgname) values "
					  "(\"%s\", %d, %d, %d, \"%s\");",
					  SP_INFO_TBL,
					  pinfo->name, pinfo->prio,
					  pinfo->focus, pinfo->timeout,
					  pinfo->term_act, pinfo->endkey_act,
					  pinfo->pkgname);
		rc = __exec(db, _sqlbuf);
		sqlite3_free(_sqlbuf);
	} else {
		_E("already exist - %s", pinfo->name);
		rc = -1;
	}

	if (rc < 0)
		_E("Fail to insert\n");

	__fini();
	return rc;
}

syspopup_info_t *_syspopup_info_get(const char *popup_name)
{
	int rc = 0;
	char *_sqlbuf;
	sqlite3_stmt *stmt;
	syspopup_info_t *pinfo = NULL;

	if (popup_name == NULL) {
		_E("no popup name");
		return NULL;
	}

	if (__init() < 0)
		return NULL;

	_sqlbuf = sqlite3_mprintf("SELECT name,prio,focus,timeout,term_act,endkey_act,pkgname "
				  "FROM %s WHERE %s='%s';",
				  SP_INFO_TBL, SP_INFO_TBL_F_NAME, popup_name);

	rc = sqlite3_prepare_v2(db, _sqlbuf, -1, &stmt, NULL);
	if (rc == SQLITE_OK) {
		rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW) {
			pinfo = (syspopup_info_t *) malloc(sizeof(syspopup_info_t));
			if (pinfo == NULL) {
				__fini();
				return NULL;
			}
			pinfo->name = strdup((char *) sqlite3_column_text(stmt, 0));
			pinfo->prio = sqlite3_column_int(stmt, 1);
			pinfo->focus = sqlite3_column_int(stmt, 2);
			pinfo->timeout = sqlite3_column_int(stmt, 3);
			pinfo->term_act = sqlite3_column_int(stmt, 4);
			pinfo->endkey_act = sqlite3_column_int(stmt, 5);
			if (sqlite3_column_text(stmt, 6) != NULL)
				pinfo->pkgname = strdup((char *) sqlite3_column_text(stmt, 6));
			else
				pinfo->pkgname = NULL;
		}
		sqlite3_finalize(stmt);
	}

	sqlite3_free(_sqlbuf);
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

