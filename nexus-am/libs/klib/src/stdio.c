#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap,fmt);
    char buf[80];
    int i,n=vsprintf(buf,fmt,ap);
    _putc('p');
    for(i=0;i<n;++i){
        _putc(buf[i]);
    }
    return i;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
#define output(A) out[cnt++]=A;
    size_t cnt=0;
    int i=0;
    double d;
    const char *p, *sval;
    char fill,num[10];
    int ival,fill_width;
    double dval;
    uintptr_t uval;
    for(p=fmt;*p!='\0';++p){
        if(*p!='%'){
            output(*p);
            continue;
        }
        fill_width=0;
        fill=' ';
        //do {
            if (*p == '0') {
            fill = '0';
            ++p;
            }
            fill_width = 0;
            ++p;
            while (*p >= '0' && *p <= '9') {
                fill_width *= 10;
                fill_width += *p - '0';
                ++p;
            }
            switch (*p) {
                case 'u':
                    uval = va_arg(ap, uint32_t);
                    i = 0;
                    while (uval > 0) {
                        num[i++] = uval % 10 + '0';
                        uval /= 10;
                    }
                    while (fill_width > i) {
                        num[i++] = fill;
                    }
                    if (i == 0) {
                        output('0');
                    } else {
                        while (i > 0) {
                            output(num[--i]);
                        }
                    }
                    break;
                case 'x':
                case 'p':
                    uval = (uintptr_t) va_arg(ap, void * );
                    i = 8;
                    while (i > 0) {
                        output(
                                (uval >> ((sizeof(void *) << 3) - 4)) < 10 ?
                                (uval >> ((sizeof(void *) << 3) - 4)) + '0' :
                                (uval >> ((sizeof(void *) << 3) - 4)) - 10 + 'a');
                        uval <<= 4;
                        --i;
                    }
                    break;
                case 'd':
                    ival = va_arg(ap, int);
                    if (ival < 0) {
                        output('-');
                        ival = -ival;
                    }
                    while (ival > 0) {
                        num[i++] = ival % 10 + '0';
                        ival /= 10;
                    }
                    while (fill_width > i) {
                        num[i++] = fill;
                    }
                    if (i == 0) {
                        output('0');
                    } else {
                        while (i > 0) {
                            output(num[--i]);
                        }
                    }
                    break;
                case 'f':
                    dval = va_arg(ap, double);
                    d=10000;
                    while (d > 0.001) {
                        int j = (int) dval / d;
                        output(j + '0');
                        dval -= ((int) dval / d) * d;
                        d /= 10;
                    }
                    break;
                case 's':
                    for (sval = va_arg(ap, char * ); *sval != '\0'; ++sval) {
                        output(*sval);
                    }
                    break;
                default:
                    if (*p < '0' || *p > '9') {
                        output(*p);
                        cnt+=vsprintf(out+cnt,"not realized\n",ap);
                        break;
                    }
            }
        //}while(0);
    }
    va_end(ap);
    output('\0');
    return cnt-1;
#undef output
}

int sprintf(char *out, const char *fmt, ...) {
    va_list ap;
    va_start(ap,fmt);
    return vsprintf(out,fmt,ap);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    va_list ap;
    va_start(ap,fmt);
    return vsprintf(out,fmt,ap);
}

#endif
