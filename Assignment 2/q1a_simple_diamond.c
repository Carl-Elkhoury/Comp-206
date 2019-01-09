#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

    if(argc!=2){
        printf("ERROR: Wrong number of arguments. One required");
        // return 0;
    }
    int input = atoi(argv[1]);
    if(input%2 == 0){
        printf("ERROR: Bad argument. Height must be positive odd integer");
    }
    for(int i=0 ; i<input ; i++){
        int spaceCount = (input/2)-i;
        int starCount;

        if(2*i+1<=input)
            starCount=2*i+1;
        else
            starCount=input-(2*i+1)%input;

        if(spaceCount<0)
            spaceCount=-spaceCount;

        for(int j=0; j<spaceCount; j++)
            printf(" ");

        for(int j=0; j<starCount; j++)
            printf("*");

        printf("\n");
    }

    return 0;
}