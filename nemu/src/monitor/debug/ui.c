#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
extern bool attach;
#if defined(DIFF_TEST)
static char nemu_cmd_line[20]="(nemu difftest:ON)";
#else
static char nemu_cmd_line[20]="(nemu difftest:OFF)";
#endif
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline(nemu_cmd_line);

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
		show_wp();
	}else{
			printf("Unknown command!\n");
	}
	return 0;
}

static int cmd_p(char *args) {
	bool success=true;
	int i=expr(args,&success);
	if(success==false){
					printf("A bad expression\n");
					return -1;
	}
	printf("%d\n",i);
	return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args){
	int i=0;//Avoid errors
	if(args==NULL||sscanf(args,"%d",&i)!=1){i=1;}
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
static int cmd_w(char *args){
	bool success=true;
	WP *temp=new_wp();
	strcpy(temp->exprr,args);
	temp->old_value=expr(temp->exprr,&success);
	return 0;
}
static int cmd_d(char *args){
	int i=0;
	sscanf(args,"%d",&i);
	free_wp_by_int(i);
	return 0;
}
static int cmd_detach(char *args) {
#ifndef DIFF_TEST
	printf("diff-test is not defined!\n");
#else
	if(attach==false)return 0;//Just to speed up
    attach=false;
    sprintf(nemu_cmd_line+15,"OFF)");
    puts("Detach successfully!");
#endif
    return 0;
}
static int cmd_attach(char *args) {
#ifndef DIFF_TEST
	printf("diff-test is not defined!\n");
#else
	if(attach==true)return 0;//Just to speed up
	attach=true;
	sprintf(nemu_cmd_line+15,"ON)");
	void difftest_attach(void);
	difftest_attach();
    void difftest_skip_clear();
    difftest_skip_clear();
	puts("attach successfully!");
#endif
	return 0;
}
static int cmd_save(char *args){
    FILE *fp=fopen(args,"w+");
    if(fp==NULL){printf("Save failed!\n");return 1;}

	assert(fwrite(&cpu,1,sizeof(CPU_state),fp)== sizeof(CPU_state));
#define PMEM_SIZE (128 * 1024 * 1024)
    assert(fwrite(guest_to_host(0),1,PMEM_SIZE,fp)==PMEM_SIZE);
#undef PMEM_SIZE
	assert(fwrite(&nemu_state,1,sizeof(nemu_state),fp)==sizeof(nemu_state));
    assert(fclose(fp)==0);
	printf("Save successfully!\n");
    return 0;
}
static int cmd_load(char *args){
	FILE *fp=fopen(args,"r");
	if(fp==NULL){printf("Load failed!\n");return 1;}

	assert(fread(&cpu,1,sizeof(CPU_state),fp)==sizeof(CPU_state));
#define PMEM_SIZE (128 * 1024 * 1024)
	assert(fread(guest_to_host(0),1,PMEM_SIZE,fp)==PMEM_SIZE);
#undef PMEM_SIZE
	assert(fread(&nemu_state,1,sizeof(nemu_state),fp)==sizeof(nemu_state));
	assert(fclose(fp)==0);
	printf("Load successfully!\n");
	if(attach==true){
		void difftest_attach(void);
		difftest_attach();
		puts("auto attach successfully!");
	}
	return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
		{"c",      "Continue the execution of the program",             cmd_c},
		{"d",      "Delete a watchpoint by its number",                 cmd_d},
		{"help",   "Display informations about all supported commands", cmd_help},
		{"info",   "Print info of registers or watchpoints",            cmd_info},
		{"p",      "calculate the value",                               cmd_p},
		{"q",      "Exit NEMU",                                         cmd_q},
		{"si",     "Step N instruction(s) exactly.",                    cmd_si},
		{"w",      "Set a watchpoint.",                                 cmd_w},
		{"x",      "Print the number at assigned address",              cmd_x},
		{"detach", "Cancel Diff-test mode",                             cmd_detach},
		{"attach", "Restart difftest mode",                             cmd_attach},
		{"save",   "Save nemu's current state",                         cmd_save},
		{"load",   "Load nemu's state from a file",                     cmd_load}
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
