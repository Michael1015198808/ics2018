#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
	size_t cnt=0;
	va_list ap;
	const char *p, *sval;
	char fill;
	int ival,fill_width;
	double dval;
	uint32_t uval;
	va_start(ap,fmt);
	for(p=fmt;*p!='\0';++p){
		if(*p!='%'){
			_putc(*p);
			continue;
		}
		fill_width=0;
		fill=' ';
re:;
		switch(*++p){
			case 'u':
				uval=va_arg(ap,uint32_t);
				int i=0;
				char num[10000];
				while(uval>0){
					num[i++]=uval%10+'0';
					uval/=10;
				}
				while(fill_width>i){
					num[i++]=fill;
				}
				if(i==0){
					_putc('0');
				}else{
					while(i>0){
						_putc(num[--i]);
					}
				}
				break;
			case 'd':
				{
				ival=va_arg(ap,int);
				if(ival<0){
					_putc('-');
					ival=-ival;
				}
				int i=0;
				char num[10000];
				while(ival>0){
					num[i++]=ival%10+'0';
					ival/=10;
				}
				while(fill_width>i){
					num[i++]=fill;
				}
				if(i==0){
					_putc('0');
				}else{
					while(i>0){
						_putc(num[--i]);
					}
				}
				}
				break;
			case 'f':
				{
				dval=va_arg(ap,double);
				double i=1000000;
				while(i>0.001){
					int j=(int)dval/i;
					_putc(j+'0');
					dval-=((int)dval/i)*i;
					i/=10;
				}
				}
				break;
			case 's':
				for(sval=va_arg(ap,char*);*sval!='\0';++sval){
					_putc(*sval);
				}
				break;
			default:
				if(*p=='0'){
					fill='0';
					++p;
				}
				fill_width=0;
				while(*p>='0'&&*p<='9'){
								fill_width*=10;
								fill_width+=*p-'0';
								++p;
				}
				--p;
				goto re;
				break;
		}
	}
	va_end(ap);
  return cnt;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
	size_t cnt=0;
	va_list ap;
	const char *p, *sval;
	int ival;
	double dval;
	//uint32_t uval;
	va_start(ap,fmt);
	for(p=fmt;*p!='\0';++p){
		if(*p!='%'){
			out[cnt++]=*p;
			continue;
		}
		switch(*++p){
			case 'd':
				{
				ival=va_arg(ap,int);
				if(ival<0){
					out[cnt++]='-';
					ival=-ival;
				}
				int i=0;
				char num[10];
				while(ival>0){
					num[i]=ival%10;
					ival/=10;
					++i;
				}
				while(i>0){
					--i;
					out[cnt++]=num[i]+'0';
				}
				}
				break;
			case 'f':
				{
				dval=va_arg(ap,double);
				double i=1000000;
				while(i>0.001){
					int j=(int)dval/i;
					out[cnt++]=j+'0';
					dval-=((int)dval/i)*i;
					i/=10;
				}
				}
				break;
			case 's':
				for(sval=va_arg(ap,char*);*sval!='\0';++sval){
					out[cnt++]=*sval;
				}
				break;
			default:
				out[cnt++]=*p;
				break;
		}
	}
	va_end(ap);
  out[cnt]='\0';
  return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
