#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
static int position;
void gen(char c){
	buf[++position]=c;
//	putchar(c);
}
void gen_num(void){
	int i;
	gen(rand()%9+'1');
	for(i=1;i<3;++i){
		gen(rand()%10+'0');
	}
}
void gen_rand_op(void){
	switch(rand()&3){
		case 0:gen('+');break;
		case 1:gen('-');break;
		case 2:gen('*');break;
		case 3:gen('/');break;
	}
}
static inline void gen_rand_expr() {
  switch (rand()%3) {
	    case 0: gen_num(); break;
	    case 1: gen('('); gen_rand_expr(); gen(')'); break;
	    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
	   }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
		position=-1;
    gen_rand_expr();
		buf[position+1]='\0';
		if(position>100){
			--i;
			continue;
		}

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -o .expr .code.c -Werror");
    if (ret != 0){
						--i;
					 	continue;
		}

    fp = popen("./.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
