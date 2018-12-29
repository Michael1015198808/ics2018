#include "trap.h"
void recur(int a){
    if(a==0)return;
    recur(a-1);
    return;
}
int main(){
    recur(9);
    return 0;
}
