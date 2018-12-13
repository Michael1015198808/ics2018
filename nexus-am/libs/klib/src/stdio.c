#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
#define printf_instructions 	size_t cnt=0;\
    output('K');\
	va_list ap;\
	int i;\
	const char *p, *sval;\
	char fill;\
	int ival,fill_width;\
	double dval;\
	uint32_t uval;\
	va_start(ap,fmt);\
	for(p=fmt;*p!='\0';++p){\
		if(*p!='%'){\
		    output(*p);\
			continue;\
		}\
		fill_width=0;\
		fill=' ';\
re:;\
		switch(*++p){\
			case 'u':\
				uval=va_arg(ap,uint32_t);\
				i=0;\
				char num[10000];\
				while(uval>0){\
					num[i++]=uval%10+'0';\
					uval/=10;\
				}\
				while(fill_width>i){\
					num[i++]=fill;\
				}\
				if(i==0){\
					output('0');\
				}else{\
					while(i>0){\
						output(num[--i]);\
					}\
				}\
				break;\
				case 'x':\
		    case 'p':\
				uval=(int32_t)va_arg(ap,void*);\
		   	func_name("0x");\
				i=8;\
				while(i>0){\
					output(\
					(uval>>((sizeof(void*)<<3)-4))<10?\
					(uval>>((sizeof(void*)<<3)-4))+'0':\
					(uval>>((sizeof(void*)<<3)-4))-10+'a');\
					uval<<=4;\
					--i;\
				}\
				break;\
			case 'd':\
				{\
				ival=va_arg(ap,int);\
				if(ival<0){\
					output('-');\
					ival=-ival;\
				}\
				int i=0;\
				char num[50];\
				while(ival>0){\
					num[i++]=ival%10+'0';\
					ival/=10;\
				}\
				while(fill_width>i){\
					num[i++]=fill;\
				}\
				if(i==0){\
					output('0');\
				}else{\
					while(i>0){\
						output(num[--i]);\
					}\
				}\
				}\
				break;\
			case 'f':\
				{\
				dval=va_arg(ap,double);\
				double i=1000000;\
				while(i>0.001){\
					int j=(int)dval/i;\
					output(j+'0');\
					dval-=((int)dval/i)*i;\
					i/=10;\
				}\
				}\
				break;\
			case 's':\
				for(sval=va_arg(ap,char*);*sval!='\0';++sval){\
					output(*sval);\
				}\
				break;\
			default:\
				if(*p<'0'||*p>'9'){\
					output(*p);\
					func_name(" not realized\n");\
				}\
				if(*p=='0'){\
					fill='0';\
					++p;\
				}\
				fill_width=0;\
				while(*p>='0'&&*p<='9'){\
								fill_width*=10;\
								fill_width+=*p-'0';\
								++p;\
				}\
				--p;\
				goto re;\
				break;\
		}\
	}\
	va_end(ap);
int printf(const char *fmt, ...) {
#define func_name(A,...) printf(A, ## __VA_ARGS__);
#define output(A) _putc(A)
printf_instructions
#undef output
#undef func_name
	return cnt;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
#define func_name(A,...) sprintf(out,A, ## __VA_ARGS__);
#define output(A) out[cnt++]=A
printf_instructions
#undef output
#undef func_name
  return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
