//To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
//To run (linux/mac): ./main.out example.bmp example_inv.bmp

//To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
//To run (win): .\main.exe example.bmp example_inv.bmp

#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

void greyscale (unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS], unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      greyscale_image[x][y] = (input_image[x][y][0]+input_image[x][y][1]+input_image[x][y][2])/3;
    } 
  }
}

void threshold (unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH], unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH]){
  for (int x = 0; x < BMP_WIDTH; x++)
  {
    for (int y = 0; y < BMP_HEIGTH; y++)
    {
      if (greyscale_image[x][y] <= 90) {
        binary_image[x][y] = 0;
      } else {
        binary_image[x][y] = 255;
      }
    } 
  }
}

void array_to_image_converter (unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH],unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]){
  for (int x = 0; x < BMP_WIDTH; x++){
    for (int y = 0; y < BMP_HEIGTH; y++){
      unsigned char rgb_color = greyscale_image[x][y];
      output_image[x][y][0] = rgb_color;
      output_image[x][y][1] = rgb_color;
      output_image[x][y][2] = rgb_color;
    }
  }
}

  //Declaring the array to store the image (unsigned char = unsigned 8 bit)
  unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
  unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH];
  unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH];
  unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

//Main function
int main(int argc, char** argv)
{
  //Checking that 2 arguments are passed
  if (argc != 3)
  {
      fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
      exit(1);
  }

  printf("Example program - 02132 - A1\n");

  //Load image from file
  read_bitmap(argv[1], input_image);
  printf("Read bitmap from file\n");

  //Run greyscale from image
  greyscale(input_image,greyscale_image);
  printf("Converted into grayscale\n");

  //Converts greyscale image to binary image
  threshold(greyscale_image, binary_image);
  printf("Converted into binary \n");

  //Convert greyscale 2D array to image
  array_to_image_converter(binary_image,output_image);
  printf("converted into output img\n");

  //Save image to file
  write_bitmap(output_image, argv[2]);

  printf("Done!\n");
  return 0;
}
