#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
	TK_LEFT, TK_RIGHT,
	TK_NUM, TK_DEREF,
	TK_REG

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
	{"-", '-'},						// minus
	{"\\*", '*'},					// multiply
	{"/", '/'},						// devide
	{"\\(", TK_LEFT},
	{"\\)", TK_RIGHT},
	{"[0123456789]+", TK_NUM},
	{"==", TK_EQ},        // equal
	{"\\$e..", TK_REG},		// regester
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
int check_parentheses(int,int);
int eval(int,int);

void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX;++ i) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[60];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

				int j=0;
        switch (rules[i].token_type) {
					case TK_NOTYPE:
						break;
					case TK_NUM:
					case TK_REG:
						for(j=0;j<substr_len;++j){
							tokens[nr_token].str[j]=e[position+j];
							if(j>30){
								return false;
							}
						}
						tokens[nr_token].str[j]='\0';
						//Falls down
          default:
						if(rules[i].token_type == '*' && (nr_token == 0 || (tokens[nr_token - 1].type != TK_NUM&&tokens[nr_token - 1].type != TK_LEFT)) ) {
						  tokens[nr_token].type = TK_DEREF;
						}else{
							tokens[nr_token].type=rules[i].token_type;
						}
						++nr_token;
						// TODO: copy string to here.
        }
        position += substr_len;

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
	printf("Make successfully\n");

  /* TODO: Insert codes to evaluate the expression. */
//	printf("%d\n",eval(0,nr_token-1));
  return eval(0,nr_token-1);
}
int eval(int p,int q) {
				  if (p > q) {
									//printf("Wrong expression!\n");
									return 0;
					}else if (p == q) {
									int i=0,j=0;
									if(tokens[p].type==TK_NUM){
										while(tokens[p].str[j]!='\0'){
											i*=10;
											i+=tokens[p].str[j]-'0';
											++j;
										}
									}else{
										for(j=0;j<8;++j){
											if(!strcmp(regsl[j],tokens[p].str+1)){
												extern CPU_state cpu;
												i=(int)cpu.gpr[j]._32;
												break;
											}
										}
									}
									return i;
					}else if (check_parentheses(p, q) == true) {
				    return eval(p + 1, q - 1);
					}else {
						int i=0,flags=0,op=i;
						for(i=p;i<q;++i){
								switch(tokens[i].type){
									case TK_LEFT:
										++flags;
										break;
									case TK_RIGHT:
										--flags;
										break;
									case '+':
										if(flags==0){
											op=i;
										}
										break;
									case '-':
										if(flags==0){
											op=i;
										}
										break;
									case '*':
										if(flags==0&&tokens[op].type!='+'&&tokens[op].type!='-'){
											op=i;
										}
										break;
									case '/':
										if(flags==0&&tokens[op].type!='+'&&tokens[op].type!='-'){
											op=i;
										}
										break;
									default:
										break;
								}
						}
						int val1 = eval(p, op - 1);
						int val2 = eval(op + 1, q);
						printf("op%d\n",op);
						switch (tokens[op].type) {
						  case '+': return val1 + val2;
						  case '-': return val1 - val2;
						  case '*': return val1 * val2;
						  case '/': return val1 / val2;
						  default: assert(0);
				  	}
					}
		return 0;
}
int check_parentheses(int p,int q){
	int flags=0,i=0;
	for(i=p;i<q;++i){
		if(tokens[i].type==TK_LEFT){
			++flags;
		}else if(tokens[i].type==TK_RIGHT){
			--flags;
		}
		if(flags==0){
			return false;
		}
	}
	if(flags==1&&tokens[i].type==TK_RIGHT){
		return true;
	}
	printf("Wrong expression!\n");
	return false;
}
