#include <stdio.h>
#include<ctype.h>

int main(){
        int i = 0, flag = 1;
        char ch = getchar();
        while(ch != EOF){
                if(!isalpha(ch)){
                        flag++;
                }
		ch = getchar();
	}
        ch = getchar();
        fprintf(stdout,"%d\n", flag);
	return 0;
}

