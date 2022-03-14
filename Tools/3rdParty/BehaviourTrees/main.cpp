#include <conio.h>
#include <stdio.h>
#include "bt_soldier.h"


int main()
{
bt_soldier bs1;

bs1.Init();

int i=0;
while(1)
	{
	printf("\n\nframe %d\n",i);
	i++;
	bs1.recalc();
	char c=_getch();
	}
return 0;
}


