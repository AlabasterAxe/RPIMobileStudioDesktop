/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "ObjTable.h"


static void *_objtable[OBJMAX];

int
ObjTable_Init(void)
{
	int idx;

	for (idx = 0; idx < OBJMAX; idx++)
		_objtable[idx] = NULL;
}

int
ObjTable_Add(void *obj)
{
	int idx;

	for (idx = 0; idx < OBJMAX; idx++) {
		if (_objtable[idx] == NULL) {
			_objtable[idx] = obj;
			return idx;
		}
	}
	return -ENFILE;
}

void *
ObjTable_Get(int od)
{
	if (od < 0 || od > (OBJMAX - 1))
		return NULL;
	return _objtable[od];
}

int
ObjTable_Remove(int od)
{
	if (od < 0 || od > (OBJMAX - 1))
		return -EBADF;
	_objtable[od] = NULL;
	return 0;
}
