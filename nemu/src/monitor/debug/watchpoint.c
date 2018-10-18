#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i+1];
  }
	wp_pool[NR_WP-1].next=NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(void){
	if(free_->next==NULL){
		assert(0);
	}
	if(head==NULL){
		head=free_;
	}
	WP *temp=free_;
	free_=free_->next;
	return temp;
}
void free_wp_by_int(int i){
	free_wp(wp_pool+i);
}
void free_wp(WP *wp){
	WP *temp=free_;
	while(temp->next!=NULL){
		temp=temp->next;
	}
	temp->next=wp;
	if(head==wp){
		head=wp->next;
	}else{
		temp=head;
		while(temp->next!=wp){
			temp=temp->next;
		}
		temp->next=wp->next;
	}
	wp->next=NULL;
}
bool check_wp(void){
	bool success=false,trash=true;
	WP *temp=head;
	while(temp!=NULL&&temp!=free_){
		int i=expr(temp->exprr,&trash);
		if(i!=temp->old_value){
			printf("Watchpoint%d :\n%s\n changes from %d to %d\n",temp->NO,temp->exprr,temp->old_value,i);
			temp->old_value=i;
			success=true;
		}
		temp=temp->next;
	}
	return success;
}
void show_wp(void){
		WP *temp=head;
		printf("Num\tvalue\t\texpression\n");
		while(temp!=NULL&&temp!=free_){
			printf("%d\t%d\t\t%s\n",temp->NO,temp->old_value,temp->exprr);
			temp=temp->next;
		}
}
