#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO,old_value;
  struct watchpoint *next;
	char exprr[50];

  /* TODO: Add more members if necessary */


} WP;
WP* new_wp(void);
void free_wp(WP *wp);

#endif
