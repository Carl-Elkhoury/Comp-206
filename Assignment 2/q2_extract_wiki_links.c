#include <stdio.h>
#include <string.h>
int main(int argc, char* argv[]) {
    FILE* file = fopen(argv[1],"r");
    if(file == NULL){
        printf("File not found\n");
        return 0;
    }
    fseek(file,0L,SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char fileData[size+1];
    fread(fileData,1,size+1,file);
    fileData[size]='\0';
    char *result = fileData;
    char target[] = "<a href=\"/wiki/";

    char *halfResult = strstr(result, target);

    while(halfResult!=NULL){
        result = strstr(halfResult, "</a>");
//        for (int i = 0; i < (result - halfResult + 4); i++)
//            printf("%c", halfResult[i]);
//        printf("\n");
        char* title = strstr(halfResult,"title=\"");
        if(title==NULL || title-halfResult> result-halfResult) {
            halfResult = strstr(result, target);
            continue;
        }
        int i=15;
        while(halfResult[i]!='\"') {
            printf("%c",halfResult[i]);
            i++;
        }
        printf("\n");
        halfResult = strstr(result, target);
    }
    fclose(file);
    return 0;
}