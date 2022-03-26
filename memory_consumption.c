#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool checkString(char* numString){
    bool flag = true;
    int i = 0;
    for(; numString[i] != '\0'; i++){
        if(numString[i] < '0' || numString[i] > '9'){
            flag = false;
            break;
        }
    }
    return flag;
}

int main(){
    char input[50];
    bool flag;
    memset(input, '\0', 50);
    printf("Enter number of MB you wanna allocate\n\
(less than 1024 and a multiple of 8):");
    scanf("%50s", input);
    flag = checkString(input);
    // check whether the input is a number
    while(!flag){
        memset(input, '\0', 50);
        printf("Please enter a number(less than 1024 and a multiple of 8):");
        scanf("%50s", input);
        flag = checkString(input);
    }
    int num = atoi(input);
    // check whether the input is within the range
    while(num >= 1024 || num % 8 != 0){
        memset(input, '\0', 50);
        printf("The number must be less than 1024,\
and a multiple of 8, please enter:");
        scanf("%50s", input);
        num = atoi(input);
    }

    char * p;
    p = (char*)malloc(num * 1024 * 1024);
    memset(p, 'a', num * 1024 * 1024);
    if(p){
        printf("Finished!\n");
        printf("Press enter to free the memory");
        getchar();
        getchar();

        free(p);
        p = NULL;
    }else{
        printf("Failed!\n");
    }
    return 0;
}
