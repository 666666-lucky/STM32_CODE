#include "list.h"

List_t List_Test;

struct xLIST_ITEM  List_Item1;
struct xLIST_ITEM  List_Item2;
struct xLIST_ITEM  List_Item3;

int main()
{
	 vListInitialise( &List_Test );
	
	  vListInitialiseItem( &List_Item1 );
    List_Item1.xItemValue = 1;
    
    /* �ڵ�2��ʼ�� */    
    vListInitialiseItem( &List_Item2 );
    List_Item2.xItemValue = 2;
    
    /* �ڵ�3��ʼ�� */
    vListInitialiseItem( &List_Item3 );
    List_Item3.xItemValue = 3;
	
	 vListInsert( &List_Test, &List_Item2 );    
    vListInsert( &List_Test, &List_Item1 );
    vListInsert( &List_Test, &List_Item3 ); 
	for(;;)
	{
	
	}
}

