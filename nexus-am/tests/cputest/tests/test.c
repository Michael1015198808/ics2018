#include <stdio.h>
#include <stdlib.h>
int Fibonacci(int x){
if (x>3){
   return Fibonacci(x-3)+2*Fibonacci(x-2);
   }else return (x-1)/2+1;
}
 
    int main(){
    int i=25,t=25;
    //while(scanf("%d%d",&i,&t)==2){
    i=i*Fibonacci(t/5+1);
    //printf("%d\n",i);}
    return 0;
}
