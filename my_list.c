#include "my_list.h"  
#include "types.h"
#include "user.h"
  
void trav_list(node_p head)  
{  
    node_p p=0;  
    p=head;  
    while(p!=0){  
        printf(1, "num:%d,ch:%c\n",p->num, p->ch);  
        p=p->next;  
    }  
    return;  
}  


node_p insert_node_to_list(node_p head,int key,char ch)  
{  
    node_p p=0;  
    p=head;   
    while(1){  	
        if(p->num==key){
			node_p q = createNode();
			q->num = p->num + 1;
			q->ch = p->ch;
			q->next = p->next;
			p->next = q;
            p->ch = ch;
			while(1)
			{
				if(q->next == 0)
					break;
				q = q->next;
				q->num++;
			}
            break;  
        } 
		if(p->next == 0)
		{
			p->next = createNode();
			p->next->num = p->num + 1;
		}
        p=p->next;  
    }     
    return head;  
}  

struct mylist* createNode()  
{  
    node_p q=0;  
     
    q=(node_p)malloc(mylistSize);  
   if(q==0)  
		;
    //printf("malloc failure!\n");  
    else{  
		q->num = 0;
        q->ch = ' ';
        q->next=0;  
            
       }  
	return q;  
}  

node_p destory_list(node_p head)  
{  
    node_p p=0;  
    p=head;  
  
    while(head!=0){  
        head=p->next;  
        free(p);  
        p=p->next;  
    }  
      
    return head;  
}  

node_p delete_node(node_p head,int key)  
{  
    node_p p=0,q=0;  
    p=head;  
    while(1){  
        if(p->next==0){  
            //printf("not found the data you want delete\n");  
            break;  
        }  
        if(head->num==key){  
            head=p->next;
			node_p m= head;
			while(1)
			{
				if(m == 0)
					break;
				m->num--;
				m = m->next;
			}
            free(p);  
            break;  
        }  
        q=p;  
        p=p->next;  
        if(p->num==key){  
            q->next=p->next; 
			node_p m= p;
			while(1)
			{
				if(m == 0)
					break;
				m->num--;
				m = m->next;
			}
            free(p);  
            break;  
        }  
    }  
      
    return head;  
}  


node_p search_node(node_p head,int key)  
{  
    node_p p=0;  
    p=head;  
    while(p!=0){  
        if(key==p->num){    
            return p;  
        }  
        else  
            p=p->next;  
    }  

	return p;
}  

char *toCharArray(node_p head)
{
	node_p p=0;  
    p=head;
	while(p!=0){  
        if(p->next == 0){    
              break;
        }  
        else  
            p=p->next;  
    }  
	int length = p->num + ((p->num / 62));
	char*ch = (char*)malloc(length);
	ch = 0;
	p = head->next;
	printf(1, "%d", length);
	for(int i = 0; i < length - 1; i++)
	{
		if(p->num > 0)
		{
			if(p->num % 62 == 0)
		{
			ch[i] = p->ch;
			i++;
			ch[i] = '\n';
		}
		else
		{
			ch[i] = p->ch;
		}
		}
		p = p->next;
	}
	ch[length - 1] = '\0';
	for(int i = 0; i < length; i++)
	{
		printf(1, "%c", ch[i]);
	}
	
	return ch;
}






