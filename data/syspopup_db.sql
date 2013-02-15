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

INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"syspopup-app",		2,1,5,0,0,"org.tizen.syspopup-app");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"incoming_call",		2,0,-1,2,2,"org.tizen.incoming_call");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"phone_lock",		2,0,-1,2,2,"org.tizen.screen_lock");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"screen_lock",		1,0,-1,0,2,"org.tizen.screen_lock");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"volume",			1,1,-1,1,1,"org.tizen.volume");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"alarm",			1,0,-1,0,2,"org.tizen.alarm");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"timer",			1,0,5,0,2,"org.tizen.timer");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"schedule_alarm",		1,0,-1,0,2,"org.tizen.schedule_alarm");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"bt-syspopup",		0,0,-1,0,0,"org.tizen.bt-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"change_select_network",	0,0,-1,0,0,"org.tizen.change_select_network");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"wifi-qs",			0,0,-1,0,0,"net.wifi-qs");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"privacy_lock",		0,0,-1,0,2,"org.tizen.privacy_lock");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"poweroff-syspopup",		1,1,-1,0,0,"org.tizen.poweroff-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"drm",			0,0,5,0,0,"org.tizen.drm-popup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"active_sync_policy",	0,0,-1,0,0,"org.tizen.active_sync_policy");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"memory_full",		0,1,5,0,0,"org.tizen.memory_full");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"lowmem-syspopup",		1,1,5,0,0,"org.tizen.lowmem-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"lowbat-syspopup",		1,1,5,0,0,"org.tizen.lowbat-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"charger_conn",		0,1,5,0,0,"org.tizen.charger_conn");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"sim_extraction",		0,1,5,0,0,"org.tizen.sim_extraction");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"time_zone_update",		0,1,5,0,0,"org.tizen.time_zone_update");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"nfc-syspopup",		0,1,3,0,0,"org.tizen.nfc-sys-popup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"push-syspopup",		2,0,5,0,0,"org.tizen.push-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"net-popup",			0,0,-1,0,0,"org.tizen.net-popup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"usb-syspopup",      	1,1,-1,2,2,"org.tizen.usb-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"ims-syspopup", 		1,1,-1,2,2,"org.tizen.ims-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"mdm-syspopup", 		1,1,5,0,0,"org.tizen.mdm-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"tickernoti-syspopup", 	1,1,-1,2,2,"org.tizen.tickernoti-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"usbotg-syspopup", 		0,1,-1,0,0,"org.tizen.usbotg-syspopup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"allshare-popup",            0,0,-1,0,0,"org.tizen.allshare-popup");
INSERT OR REPLACE INTO "syspopup_info" VALUES(NULL,"mmc-syspopup",  		0,1,-1,0,0,"org.tizen.mmc-syspopup");
