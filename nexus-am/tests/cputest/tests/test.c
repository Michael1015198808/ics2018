#include "trap.h"
void re(int a){
    if(a==0)return;
    re(a-1);
    re(a-1);
}
int main(){
    re(5);
    return 0;
}
