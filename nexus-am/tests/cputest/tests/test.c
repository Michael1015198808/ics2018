#include "trap.h"
int re(int a){
    if(a<=0)return 0;
    return 1+re(a-(a&1)-1);
}
int main(){
    printf("%d",re(5));
    return 0;
}
