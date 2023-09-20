// To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
// To run (linux/mac): ./main.out samples/easy/2EASY.bmp output.bmp

// To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
// To run (win): .\main.exe example.bmp output.bmp

#include "cbmp.h"
#include <stdio.h>
#include <stdlib.h>

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
int capture_area = 14;
int thresholdint = 90;
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char binary_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char eroded_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char red_cross_image[BMP_WIDTH][BMP_HEIGTH];
unsigned char eroded_output_images[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
int Captured_spots = 0;

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
      if (greyscale_image[x][y] <= thresholdint) {
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

void capture_part_2(int x, int y,
                    unsigned char detect_spots[BMP_WIDTH][BMP_HEIGTH]) {
  for (int m = 0; m < capture_area; m++) {
    for (int n = 0; n < capture_area; n++) {
      if (detect_spots[x + m][y + n] == 255) {
        Captured_spots++;
        for (int o = 0; o < capture_area; o++) {
          for (int p = 0; p < capture_area; p++) {
            eroded_image[x + o][y + p] = 0;
            detect_spots[x + o][y + p] = 0;
            red_cross_image[x + 5][y + 5] = 1;
          }
        }
        printf("Captured spot %i\n", Captured_spots);
      }
    }
  }
}

void capture(unsigned char detect_spots[BMP_WIDTH][BMP_HEIGTH]) {
  for (int i = 0; i < BMP_WIDTH - capture_area; i++) {
    for (int j = 0; j < BMP_HEIGTH - capture_area; j++) {
      int lock = 1;
      for (int k = 0; k < capture_area; k++) {
        for (int l = 0; l < capture_area; l++) {
          if (detect_spots[i + 0][j + l] == 255 ||
              detect_spots[i + capture_area - 1][j + l] == 255 ||
              detect_spots[i + k][j + 0] == 255 ||
              detect_spots[i + k][j + capture_area - 1] == 255) {
            lock = 0;
            continue;
          }
        }
      }
      if (lock) {
        capture_part_2(i, j, detect_spots);
      }
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
    for (int x = 0; x < BMP_WIDTH; x++) {
      for (int y = 0; y < BMP_HEIGTH; y++) {
        eroded_image[x][y] = binary[x][y];
        if (binary[x][y] == 255 &&
            (binary[x - 1][y] == 0 || binary[x + 1][y] == 0 ||
             binary[x][y - 1] == 0 || binary[x][y + 1] == 0)) {
          allBlack = 1;
          eroded_image[x][y] = 0;
        }
      }
    }
    capture(eroded_image);
    for (int i = 0; i < BMP_WIDTH; i++) {
      for (int j = 0; j < BMP_HEIGTH; j++) {
        eroded_image[0][j] = 0;
        eroded_image[BMP_WIDTH - 1][j] = 0;
        eroded_image[i][0] = 0;
        eroded_image[i][BMP_HEIGTH - 1] = 0;
        binary[i][j] = eroded_image[i][j];
      }
    }
    printf("Eroded image %i times\n", counter);
    char filename[256];
    sprintf(filename, "./eroded_images/eroded_image%i.bmp", counter);
    array_to_image_converter(eroded_image, eroded_output_images);
    write_bitmap(eroded_output_images, filename);
  }
}

void red_cross(unsigned char input[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS],
               unsigned char red_cross[BMP_WIDTH][BMP_HEIGTH]) {
  
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGTH; j++) {

      if (red_cross[i-2][j-2] == 1) {
        int cross_size=10;
        int cross_thickness=1;

        for (int k = -cross_size; k <= cross_size; k++) {
          input[i+k][j][0] = 255;
          input[i+k][j][1] = 0;
          input[i+k][j][2] = 0;
          for (int l = -cross_size; l <= cross_size; l++) {
          input[i][j+l][0] = 255;
          input[i][j+l][1] = 0;
          input[i][j+l][2] = 0;
          for (int m = -cross_thickness; m <= cross_thickness; m++) {
          input[i+k][j+m][0] = 255;
          input[i+k][j+m][1] = 0;
          input[i+k][j+m][2] = 0;

          input[i+m][j+l][0] = 255;
          input[i+m][j+l][1] = 0;
          input[i+m][j+l][2] = 0;
          }
        }
      }
    }
  }

  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGTH; j++) {
      for (int k = 0; k < BMP_CHANNELS; k++) {
        output_image[i][j][k] = input[i][j][k];
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

  // erodes image
  erode(binary_image, eroded_image);
  printf("Eroded image \n");

  // Prints red cross on original image
  red_cross(input_image, red_cross_image);
  printf("Printed red cross' \n");

  // // Convert greyscale 2D array to image
  // array_to_image_converter(binary_image, output_image);
  // printf("converted into output img\n");

  // Save image to file
  write_bitmap(output_image, argv[2]);

  printf("Done!\n");
  return 0;
}
