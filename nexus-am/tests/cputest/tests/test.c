#include <stdio.h>
//
void move(char x, char y,char z,int d) {
    if (d>0){
    move(x,z,y,d-1);
	static int i=0;
    i=i+1;
    move(y,z,x,d-1);
}
}
int main(){
	char a='a';
	char b='b';
	char c='c';
	move(a,b,c,5);
}
