/* FILE: A3_solutions.c is where you will code your answers for Assignment 3.
 *
 * Each of the functions below can be considered a start for you. They have
 * the correct specification and are set up correctly with the header file to
 * be run by the tester programs.
 *
 * You should leave all of the code as is, especially making sure not to change
 * any return types, function name, or argument lists, as this will break
 * the automated testing.
 *
 * Your code should only go within the sections surrounded by
 * comments like "REPLACE EVERTHING FROM HERE... TO HERE.
 *
 * The assignment document and the header A3_solutions.h should help
 * to find out how to complete and test the functions. Good luck!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "A3_provided_functions.h"

unsigned char*
bmp_open( char* bmp_filename,        unsigned int *width,
          unsigned int *height,      unsigned int *bits_per_pixel,
          unsigned int *padding,     unsigned int *data_size,
          unsigned int *data_offset                                  )
{
    unsigned char *img_data=NULL;
    if(bmp_filename == NULL || width == NULL || height == NULL || padding == NULL || data_size == NULL || data_offset == NULL || bits_per_pixel == NULL)
        return NULL;
    // REPLACE EVERYTHING FROM HERE
    FILE *file = fopen(bmp_filename,"rb");
    if(file == NULL)
        return NULL;
    unsigned char* data = (unsigned char*) malloc(sizeof(char)*55);
    data[54]='\0';
    fread(data, sizeof(char), 54, file);
    rewind(file);
    *width = *(unsigned int*) &data[18];
    *height = *(unsigned int*) &data[22];
    *bits_per_pixel = *(unsigned int*)&data[28];
    *bits_per_pixel &= 65535;
    *data_size = (*(unsigned int*)&data[34]);
    *data_offset = *(unsigned int*) &data[10];
    *data_size += *data_offset;
    int remainder= (*bits_per_pixel * *width) % 32;

    if(remainder==0)
        *padding=0;
    else
        *padding= (32-remainder)/8;


    img_data= (char*) malloc(sizeof(char) * *data_size);
    fread(img_data, sizeof(char), *data_size, file );
    // TO HERE!
    return img_data;
}

void
bmp_close( unsigned char **img_data )
{
    // REPLACE EVERYTHING FROM HERE
    if(img_data == NULL)
        return;
    if(*img_data == NULL) {
        return;
    }
    if(img_data[0][0] != 'B' || img_data[0][1] != 'M')
        return;

    free(*img_data);
    *img_data = NULL;
    // TO HERE!
}

unsigned char***
bmp_scale( unsigned char*** pixel_array, unsigned char* header_data, unsigned int header_size,
           unsigned int* width, unsigned int* height, unsigned int num_colors,
           float scale )
{
    unsigned char*** new_pixel_array = NULL;

    if(scale<=0 || height==NULL || *height<=0 || width == NULL || *width<=0 || header_size == 0 || num_colors==0 || header_data==NULL
       || pixel_array == NULL || *pixel_array ==NULL || **pixel_array == NULL)
        return NULL;

    int newHeight = *height * scale;
    int newWidth = *width * scale;
    new_pixel_array = ( unsigned char***) malloc( newHeight * sizeof(char**));

    for(int i=0; i<newHeight ; i++)
        new_pixel_array[i] = (unsigned char**) malloc(newWidth * sizeof(char*));

    for(int i=0; i<newHeight; i++)
        for(int j=0; j<newWidth ; j++)
            new_pixel_array[i][j] = (unsigned char*) malloc(num_colors * sizeof(char));

    for(int row=0; row<newHeight; row++)
        for(int column=0; column<newWidth; column++)
            for(int numcol=0 ; numcol<num_colors; numcol++ )
                new_pixel_array[row][column][numcol] = pixel_array[(int)(row/scale)][(int)(column/scale)][numcol];

    *height=newHeight;
    *width=newWidth;
    int* imageSize = (int*) &header_data[34];
    *(int*) &header_data[18] = *width;
    *(int*) &header_data[22] = *height;
    *imageSize = *width * *height * num_colors;
    // TO HERE!
    return new_pixel_array;
}

int bmp_collage( char* background_image_filename,     char* foreground_image_filename,
                 char* output_collage_image_filename, int row_offset,
                 int col_offset,                      float scale )
{
    unsigned char* header_data;
    unsigned int header_size , width, height, num_colors;
    unsigned char*** background = bmp_to_3D_array( background_image_filename,
            &header_data, &header_size, &width, &height, &num_colors );
    if(background_image_filename == NULL || foreground_image_filename == NULL || output_collage_image_filename == NULL){
        printf("input is not valid");
        return -1;
    }
    if(background == NULL){
        printf("Couldn't read background image");
        return -1;
    }
    if(num_colors != 4){
        printf("The bits per pixel of background image has to be equal to 32");
        return -1;
    }


    unsigned char* forHeader_data;
    unsigned int forHeader_size , forWidth, forHeight, forNum_colors;
    unsigned char*** foreground = bmp_to_3D_array(foreground_image_filename,
            &forHeader_data, &forHeader_size, &forWidth, &forHeight, &forNum_colors);

    if(foreground == NULL){
        printf("Couldn't read foreground image");
        return -1;
    }
    if(forNum_colors != 4){
        printf("The bits per pixel of foreground image has to be equal to 32");
        return -1;
    }

    foreground= bmp_scale(foreground, forHeader_data, forHeader_size, &forWidth, &forHeight, forNum_colors, scale);
    if(foreground == NULL){
        printf("Error with the scaling of the foreground");
        return -1;
    }
    if((forHeight+row_offset) >height || (forWidth+col_offset)>width) {
        printf("The foreground is too big to fit in the background");
        return -1;
    }
    for(int i=0; i<forHeight; i++){
        for(int j=0; j<forWidth; j++){
            if(foreground[i][j][0] == 0) {
                continue;
            }
            for(int z=0 ; z<4; z++)
                background[i+row_offset][j+col_offset][z] = foreground[i][j][z];
        }
    }
    if(bmp_from_3D_array(output_collage_image_filename,header_data, header_size, background, width, height, num_colors) == -1)
        return -1;

    // TO HERE!
    return 0;
}

