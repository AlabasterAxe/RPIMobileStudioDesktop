/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#ifndef _OBJTABLE_H
#define _OBJTABLE_H

#include <stdio.h>
#include <stdlib.h>

#define OBJMAX 1024

int ObjTable_Init(void);
int ObjTable_Add(void *obj);
void *ObjTable_Get(int od);
int ObjTable_Remove(int od);

#endif /* _OBJTABLE_H */
