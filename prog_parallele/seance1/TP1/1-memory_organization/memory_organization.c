#include <stdio.h>
#include <stdlib.h>

int x;

int function1 (void)
{
    static int z;
    
    z = x;
    
    return (z);
}

int main(int argc, char **argv) 
{
    int y;
	  int loop = 0;
    char *str;
	  char cpy_str[] = "Ce cours est genial! Maintenant je sais reconnaitre les zones memoires ou sont situees mes variables";

    y = 4;
    x = y;

    str = malloc(100*sizeof(char));
    for (loop = 0; loop < 100; ++loop)
		str[loop] = cpy_str[loop];
	  printf("str : %s\n", str);
    free(str);
    
    loop = function1 ();
    
    return 0;
}