PRAGMA journal_mode = PERSIST;

/* prio : 0~2 (0:low, 2:high) */
/* focus : 0~1 (0:focus, 1:unfocus) */
/* timeout : -1 ~ integer value (-1:infinite, 0:don't use) */
/* term_act : 0~2(0:TERM, 1:HIDE, 2:IGNORE) */
/* endkey_act : 0~2(0:TERM, 1:HIDE, 2:IGNORE) */
CREATE TABLE IF NOT EXISTS syspopup_info (
	id INTEGER PRIMARY KEY,
	name TEXT UNIQUE NOT NULL,
	prio INTEGER,
	focus INTEGER,
	timeout INTEGER,
	term_act INTEGER,
	endkey_act INTEGER,
	pkgname TEXT
);

INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"volume",                    1,1,-1,1,1,"org.tizen.windicator");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"powerkey-syspopup",         2,0,-1,0,0,"org.tizen.powerkey-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"lowmem-syspopup",           0,0,5,0,0,"org.tizen.lowmem-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"battery-syspopup",           0,0,5,0,0,"org.tizen.battery-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"system-syspopup",           1,0,-1,0,0,"org.tizen.system-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"safetyvolume-syspopup",     2,1,-1,0,0,"org.tizen.safetyvolume-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"crash-syspopup",               1,0,-1,0,0,"org.tizen.crash-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"wc-syspopup",               0,0,-1,0,0,"net.wc-syspopup");
