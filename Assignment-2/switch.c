#include <stdio.h>
#include <ctype.h>

int main(){
        char ch = getchar();
        while(ch != EOF){
                if(islower(ch))
                       fprintf(stdout, "%c", toupper(ch));
                else
                        fprintf(stdout, "%c", tolower(ch));
		ch = getchar();
	}
	getchar();
	return 0;
}
