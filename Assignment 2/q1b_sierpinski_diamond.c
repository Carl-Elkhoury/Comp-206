
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void printArray(char **input, int height){
    for (int j = 0; j < height / 2+1; j++){
        for(int i=0 ; i<height;i++)
            printf("%c", input[i][j]);
        printf("\n");
    }
}

void printArrayR(char **input, int height){
    for(int j=height/2-1; j>=0 ;j--) {
        for (int i = 0; i < height; i++)
            printf("%c", input[i][j]);
        printf("\n");
    }
}

void insertTriangle(char** input, int topPoint[], int height){
    int star =1;
    int x=topPoint[0];
    for(int i=0; i<height/2+1; i++){
        for(int a=0; a < star; a++){
            input[x + a-1][topPoint[1] + i] = '*';
        }
        star += 2;
        x--;
    }
}

void copyTriangle(char** input, int topLeft[], int rightBottom[], int leftType){

    int yDestination;
    int xDestination;
    if(leftType)
        for(int i=rightBottom[0], xDestination=(topLeft[0]+rightBottom[0])/2-1; i>=topLeft[0] ;i--,xDestination--)
            for (int j = 0, yDestination =rightBottom[1]+1 ; j <= rightBottom[1]; j++,yDestination++)
                 input[xDestination][yDestination] = input[i][j];
    else
        for(int i=rightBottom[0], xDestination=(topLeft[0]+rightBottom[0])/2+1; i>=topLeft[0] ;i--,xDestination++)
            for (int j = 0, yDestination =rightBottom[1]+1 ; j <= rightBottom[1]; j++,yDestination++)
                input[xDestination][yDestination] = input[i][j];

}



void Print_row(int row, int height, int level, char **input, int originalHeight){

    if(level !=1){
        Print_row(row, height/2, --level, input, originalHeight);
        return;
    }else{
//        if(height%2==0)
//            height--;
        int leftTop[2]={originalHeight/2-(height/2+1)*(int)pow(2,row-1)+1,0};
        int rightBottom[2]= {originalHeight/2+(height/2+1)*(int)pow(2,row-1)-1, (height/2+1)*(int)pow(2,row-1)-1};
        if(rightBottom[0]-leftTop[0]>originalHeight/2)
            return;
//        printf("leftTop: x=%i y=%i\nrightTop: x=%i y=%i\n", leftTop[0],leftTop[1],rightBottom[0],rightBottom[1]);
        if(input[originalHeight/2+1][0] == ' ') {
            int topPoint[]={originalHeight / 2 + 1, 0};
            insertTriangle(input,topPoint, height);
        }
//        printArray(input,originalHeight);
        copyTriangle(input,leftTop,rightBottom,1);
//        printArray(input,originalHeight);
        copyTriangle(input,leftTop,rightBottom,0);
//        printArray(input,originalHeight);
        Print_row(row+1,height,level,input,originalHeight);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("ERROR: Wrong number of arguments. Two required");
        return 0;
    }
    int height = atoi(argv[1]);
    int layers = atoi(argv[2]);
    if (height % 2 == 0) {
        printf("ERROR: Bad argument. Height must be positive odd integer");
        return 0;
    }

    int temp =height;
    for(int a=1 ; a<layers; a++)
        temp/=2;
    if(!temp || temp%2==0){
        printf("ERROR: Height does not allow evenly dividing requested number of levels");
        return 0;
    }
    if (layers != 1 && (ceil(log2(height/2+1)) != floor(log2(height/2+1))|| (height/2+1 < (int)pow(2, layers-1)))){
        printf("ERROR: Height does not allow evenly dividing requested number of levels\n");
        return(0);
    }
    char **input= (char**) malloc(sizeof(char*) * height);
    for (int i = 0; i < height; i++)
        input[i] = (char *) malloc(sizeof(char) * ((height / 2) + 1));

    for (int i = 0; i < height; i++)
        for (int j = 0; j < height / 2 + 1; j++)
            input[i][j] = ' ';


    int topPoint[] = {height / 2 + 1, 0};
    int leftTop[2];
    leftTop[1]=0;
    int rightBottom[2];
    Print_row(1, height, layers, input, height);
    printArray(input,height);
    printArrayR(input,height);
}