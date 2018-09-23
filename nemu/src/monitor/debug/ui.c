#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_help(char *args);

static int cmd_info(char *args){
	int i=0;
	if(args==NULL){
		printf("info r--print the status of registers\ninfo w--print the status of watchpoints");
	}
	if(strcmp(args,"r") == 0) {
		for(i=0;i<8;++i){
			printf("R[%s] is 0x%08x\t%d\n",regsl[i],cpu.gpr[i]._32,cpu.gpr[i]._32);
		}
	}else if(strcmp(args,"w")==0){
    printf("Haven't done!\n");
	}else{
			printf("Unknown command!\n");
	}
	return 0;
}

static int cmd_p(char *args) {
	bool success=true;
	expr(args,&success);
	if(success==false){
					printf("A bad expression\n");
					return -1;
	}
	return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args){
	int i;
	if(args==NULL){
					i=1;
					printf("no argument.\n");
	}else{
					sscanf(args,"%d",&i);
	}
	cpu_exec(i);
	return 0;
}

static int cmd_x(char *args){
	int i,n,addr;
	if(args==NULL){
		printf("x N addr--print N elements of 4 bytes start at addr\n");
		return 0;
	}
	sscanf(args,"%d%x",&n,&addr);
	for(i=0;i<n;++i){
		if((i&3)==0){
			printf("\n0x%08x:",addr);
		}
		printf("\t0x%08x",vaddr_read(addr,4));
		addr+=4;
	}
	putchar('\n');
	return 0;
}
static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "c", "Continue the execution of the program", cmd_c},
  { "help", "Display informations about all supported commands", cmd_help},
	{ "info", "Print info of registers or watchpoints", cmd_info},
	{ "p", "calculate the value", cmd_p},
  { "q", "Exit NEMU", cmd_q},
  { "si", "Step N instruction(s) exactly.", cmd_si},
	{ "x", "Print the number at assigned address", cmd_x},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
