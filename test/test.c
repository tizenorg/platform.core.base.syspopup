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
#include <stdlib.h>
#include <string.h>
#include "syspopup_caller.h"

void usage()
{
	printf("[usage] sp_test launch/destroy popup_name..."
				"(key1,val1,key2,val2,...)\n");
}

int main(int argc, char **argv)
{
	bundle *b;
	int i;

	if (argc < 2) {
		usage();
		return -1;
	}

	if (strcmp(argv[1], "launch") == 0) {
		if ((argc < 3) || (argc % 2 == 0)) {
			usage();
			return -1;
		}

		b = bundle_create();
		for (i = 3; i < argc; i = i + 2)
			bundle_add(b, argv[i], argv[i + 1]);
		syspopup_launch(argv[2], b);
		bundle_free(b);
	} else if (strcmp(argv[1], "destroy") == 0) {
		syspopup_destroy_all();
	} else {
		usage();
	}

	return 0;
}

