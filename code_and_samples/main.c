// To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
// To run (linux/mac): ./main.out example.bmp example_inv.bmp

// To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
// To run (win): .\main.exe example.bmp example_inv.bmp

#include "cbmp.h"
#include <stdio.h>
#include <stdlib.h>

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char spot_image[12][12][BMP_CHANNELS];

void greyscale(unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS],
               unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH]) {
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGTH; y++) {
      greyscale_image[x][y] =
          (input_image[x][y][0] + input_image[x][y][1] + input_image[x][y][2]) /
          3;
    }
  }
}

void threshold(unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH],
               unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH]) {
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGTH; y++) {
      if (greyscale_image[x][y] <= 90) {
        binary_image[x][y] = 0;
      } else {
        binary_image[x][y] = 255;
      }
    }
  }
}

void array_to_image_converter(
    unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH],
    unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS]) {
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGTH; y++) {
      unsigned char rgb_color = greyscale_image[x][y];
      output_image[x][y][0] = rgb_color;
      output_image[x][y][1] = rgb_color;
      output_image[x][y][2] = rgb_color;
    }
  }
}

void array_to_image_converter_12x12(
    unsigned char greyscale_image[12][12],
    unsigned char output_image[12][12][BMP_CHANNELS]) {
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGTH; y++) {
      unsigned char rgb_color = greyscale_image[x][y];
      output_image[x][y][0] = rgb_color;
      output_image[x][y][1] = rgb_color;
      output_image[x][y][2] = rgb_color;
    }
  }
}

void erode(unsigned char binary[BMP_WIDTH][BMP_HEIGTH],
           unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH]) {
  int allBlack = 1;
  int counter = 0;
  while (allBlack) {
    counter++;
    allBlack = 0;
    for (int x = 1; x < BMP_WIDTH; x++) {
      for (int y = 1; y < BMP_HEIGTH; y++) {
        eroded_image[x][y] = binary[x][y];
        if (binary[x][y] == 255 &&
            (binary[x - 1][y] == 0 || binary[x + 1][y] == 0 ||
             binary[x][y - 1] == 0 || binary[x][y + 1] == 0)) {
          allBlack = 1;
          eroded_image[x][y] = 0;
        }
      }
    }
    for (int i = 0; i < BMP_WIDTH; i++) {
      for (int j = 0; j < BMP_HEIGTH; j++) {
        binary[i][j] = eroded_image[i][j];
      }
    }
    char filename[256];
    sprintf(filename, "./eroded_images/eroded_image%i.bmp", counter);
    array_to_image_converter(eroded_image, output_image);
    write_bitmap(output_image, filename);
  }
}

void capture(unsigned char detect_spots[BMP_WIDTH][BMP_HEIGTH],
             unsigned char capture_spots[12][12]) {
  int counter = 0;
  for (int i = 0; i < BMP_WIDTH - 12; i++) {
    for (int j = 0; j < BMP_HEIGTH - 12; j++) {

      for (int k = 0; i < 12; i++) {
        for (int l = 0; j < 12; j++) {
          capture_spots[k][l] = detect_spots[k][l];
          if (detect_spots[0][l] == 255 || detect_spots[11][l] == 255 ||
              detect_spots[k][0] == 255 || detect_spots[k][11] == 255) {
            continue;
          } else if (detect_spots[k][l] == 255) {
            counter++;
            char filename[256];
            sprintf(filename, "./Captured_images/captured_image%i.bmp",
                    counter);
            array_to_image_converter_12x12(capture_spots, spot_image);
            write_bitmap(spot_image, filename);
          }
        }
      }
    }
  }
}

// Main function
int main(int argc, char **argv) {
  // Checking that 2 arguments are passed
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <output file path> <output file path>\n",
            argv[0]);
    exit(1);
  }

  printf("Example program - 02132 - A1\n");

  // Load image from file
  read_bitmap(argv[1], input_image);
  printf("Read bitmap from file\n");

  // Run greyscale from image
  greyscale(input_image, greyscale_image);
  printf("Converted into grayscale\n");

  // Converts greyscale image to binary image
  threshold(greyscale_image, binary_image);
  printf("Converted into binary \n");

  erode(binary_image, eroded_image);

  // Convert greyscale 2D array to image
  array_to_image_converter(eroded_image, output_image);
  printf("converted into output img\n");

  // Save image to file
  write_bitmap(output_image, argv[2]);

  // Convert greyscale 2D array to image
  array_to_image_converter(binary_image, output_image);
  printf("converted into output img\n");

  // Save image to file
  write_bitmap(output_image, argv[2] + 1);

  printf("Done!\n");
  return 0;
}
