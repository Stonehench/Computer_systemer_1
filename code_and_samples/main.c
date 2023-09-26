// To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
// To run (linux/mac): ./main.out samples/easy/2EASY.bmp output.bmp

// To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
// To run (win): .\main.exe example.bmp output.bmp

#include "cbmp.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
int capture_area = 14;
int circleThreshold = 18;
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char bitmap2D_1[BMP_WIDTH][BMP_HEIGHT];
unsigned char bitmap2D_2[BMP_WIDTH][BMP_HEIGHT];
unsigned char bitmap2D_3[BMP_WIDTH][BMP_HEIGHT];

unsigned char eroded_output_images[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];

int Captured_spots = 0;

void calculateHistogram(unsigned char bitmap[BMP_WIDTH][BMP_HEIGHT],
                        int histogram[256]) {
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      histogram[bitmap[i][j]]++;
    }
  }
}

int getTotalPixelCount(unsigned char bitmap[BMP_WIDTH][BMP_HEIGHT]) {
  return BMP_WIDTH * BMP_HEIGHT;
}

void calculateCDF(int histogram[256], double cdf[256], int totalPixelCount) {
  cdf[0] = (double)histogram[0] / totalPixelCount;
  for (int i = 1; i < 256; i++) {
    cdf[i] = cdf[i - 1] + (double)histogram[i] / totalPixelCount;
  }
}

double calculateMean(unsigned char bitmap[BMP_WIDTH][BMP_HEIGHT],
                     int totalPixelCount) {
  double mean = 0.0;
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      mean += (double)bitmap[i][j] / totalPixelCount;
    }
  }
  return mean;
}

int calculateOtsuThreshold(unsigned char bitmap[BMP_WIDTH][BMP_HEIGHT]) {
  int histogram[256] = {0};
  calculateHistogram(bitmap, histogram);

  int totalPixelCount = getTotalPixelCount(bitmap);

  double cdf[256];
  calculateCDF(histogram, cdf, totalPixelCount);

  double mean = calculateMean(bitmap, totalPixelCount);

  double maxVariance = 0.0;
  int threshold = 0;

  for (int i = 0; i < 256; i++) {
    double p0 = cdf[i];
    double p1 = 1.0 - cdf[i];
    double mu0 = 0.0;
    double mu1 = 0.0;

    if (p0 > 0.0) {
      mu0 = (mean - cdf[i - 1]) / p0;
    }
    if (p1 > 0.0) {
      mu1 = (mean - cdf[i - 1]) / p1;
    }

    double variance = p0 * p1 * (mu0 - mu1) * (mu0 - mu1);
    if (variance > maxVariance) {
      maxVariance = variance;
      threshold = i;
    }
  }

  return threshold;
}

void greyscale(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS],
               unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT],
               unsigned char Otsu_image[BMP_WIDTH][BMP_HEIGHT]) {
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGHT; y++) {
      unsigned char r = input_image[x][y][0];
      unsigned char g = input_image[x][y][1];
      unsigned char b = input_image[x][y][2];
      greyscale_image[x][y] = (r + g + b) / 3;
      Otsu_image[x][y] = (unsigned char)(r + g + b) / 3.0;
    }
  }
}

void threshold(unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT],
               unsigned char binary_image[BMP_WIDTH][BMP_HEIGHT],
               unsigned char Otsu_image[BMP_WIDTH][BMP_HEIGHT]) {
  int thresholdInt = calculateOtsuThreshold(Otsu_image);
  printf("%i\n", thresholdInt);
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGHT; y++) {
      if (greyscale_image[x][y] < thresholdInt) {
        binary_image[x][y] = 0;
      } else {
        binary_image[x][y] = 255;
      }
    }
  }
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGHT; y++) {
      greyscale_image[x][y] = binary_image[x][y];
    }
  }
}

void array_to_image_converter(
    unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT],
    unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]) {
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGHT; y++) {
      unsigned char rgb_color = greyscale_image[x][y];
      output_image[x][y][0] = rgb_color;
      output_image[x][y][1] = rgb_color;
      output_image[x][y][2] = rgb_color;
    }
  }
}

void capture_part_2(int x, int y,
                    unsigned char detect_spots[BMP_WIDTH][BMP_HEIGHT]) {
  for (int m = 0; m < capture_area; m++) {
    for (int n = 0; n < capture_area; n++) {
      if (detect_spots[x + m][y + n] == 255) {
        Captured_spots++;
        for (int o = 0; o < capture_area; o++) {
          for (int p = 0; p < capture_area; p++) {
            bitmap2D_2[x + o][y + p] = 0;
            detect_spots[x + o][y + p] = 0;
            input_image[x + 6][y + 6][0] = 105;
            input_image[x + 6][y + 6][1] = 0;
            input_image[x + 6][y + 6][2] = 0;
          }
        }
        printf("Captured spot %i\n", Captured_spots);
      }
    }
  }
}

void capture(unsigned char detect_spots[BMP_WIDTH][BMP_HEIGHT]) {
  for (int i = 0; i < BMP_WIDTH - capture_area; i++) {
    for (int j = 0; j < BMP_HEIGHT - capture_area; j++) {
      int lock = 1;
      for (int k = 0; k < capture_area && lock; k++) {
        for (int l = 0; l < capture_area && lock; l++) {
          if (detect_spots[i + 0][j + l] == 255 ||
              detect_spots[i + capture_area - 1][j + l] == 255 ||
              detect_spots[i + k][j + 0] == 255 ||
              detect_spots[i + k][j + capture_area - 1] == 255) {
            lock = 0;
          }
        }
      }
      if (lock) {
        capture_part_2(i, j, detect_spots);
        j += capture_area - 1;
      }
    }
  }
}

// Function to calculate the squared Euclidean distance
double squaredEuclideanDistance(int x1, int y1, int x2, int y2) {
  int dx = x1 - x2;
  int dy = y1 - y2;
  return dx * dx + dy * dy;
}

// Function to compute the distance transform of a binary image
void watershed_segmentation(unsigned char binaryImage[BMP_WIDTH][BMP_HEIGHT]) {
  // Initialize the distance map to a maximum value
  int maxDistance = BMP_WIDTH * BMP_HEIGHT;
  // Iterate through each pixel in the distance map
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      if (binaryImage[i][j] == 255) { // If it's a foreground pixel
        unsigned char min_neighbor_distance = maxDistance;

        // Check neighbors in a 3x3 window
        for (int x = -1; x <= 1; x++) {
          for (int y = -1; y <= 1; y++) {
            int nx = i + x;
            int ny = j + y;

            if (nx >= 0 && nx < BMP_WIDTH && ny >= 0 && ny < BMP_HEIGHT) {
              if (binaryImage[nx][ny] == 0) { // If it's a background pixel
                unsigned int dist = squaredEuclideanDistance(i, j, nx, ny);
                if (dist < min_neighbor_distance) {
                  min_neighbor_distance = (unsigned char)dist;
                }
              }
            }
          }
        }

        if (min_neighbor_distance == 1) {
          // This is a ridge pixel, label it with a unique label
          bitmap2D_1[i][j] = 0;
        } 
      }
    }
  }
}

// Structure to represent a pixel with its properties
typedef struct {
  int x, y;  // Pixel coordinates
  int value; // Distance transform value
  int label; // Segment label
} Pixel;

// int comparePixels(const void *a, const void *b) {
//   const Pixel *pixelA = (const Pixel *)a;
//   const Pixel *pixelB = (const Pixel *)b;

//   return pixelA->value - pixelB->value;
// }

// // Function to perform watershed segmentation
// void watershedSegmentation(unsigned char distanceMap[BMP_WIDTH][BMP_HEIGHT]) {
//   // Create an array to store pixels and their properties
//   Pixel *pixels = (Pixel *)malloc(BMP_WIDTH * BMP_HEIGHT * sizeof(Pixel));

//   // Initialize the pixel array with distance map values
//   int numPixels = 0;
//   for (int x = 0; x < BMP_WIDTH; x++) {
//     for (int y = 0; y < BMP_HEIGHT; y++) {
//       pixels[numPixels].x = x;
//       pixels[numPixels].y = y;
//       pixels[numPixels].value = distanceMap[x][y];
//       pixels[numPixels].label = -1; // Initialize labels to -1
//       numPixels++;
//     }
//   }

//   // Sort the pixels based on their distance values (ascending order)
//   qsort(pixels, numPixels, sizeof(Pixel), comparePixels);

//   // Watershed segmentation
//   int currentLabel = 0;
//   for (int i = 0; i < numPixels; i++) {
//     Pixel *pixel = &pixels[i];

//     // If the pixel is not assigned to a segment
//     if (pixel->label == -1) {
//       currentLabel++; // Start a new segment
//       pixel->label = currentLabel;

//       // Perform region growing
//       for (int j = i + 1; j < numPixels; j++) {
//         Pixel *neighbor = &pixels[j];

//         // Check if the neighbor is unassigned and within a certain threshold
//         // distance
//         if (neighbor->label == -1 && abs(pixel->value - neighbor->value) <= 1) {
//           neighbor->label = currentLabel;
//         }
//       }
//     }
//   }

//   // Free memory
//   free(pixels);
// }

void erode(unsigned char binary[BMP_WIDTH][BMP_HEIGHT],
           unsigned char eroded_image[BMP_WIDTH][BMP_HEIGHT]) {
  int allBlack = 1;
  int counter = 0;
  while (allBlack) {
    counter++;
    allBlack = 0;
    for (int x = 0; x < BMP_WIDTH; x++) {
      for (int y = 0; y < BMP_HEIGHT; y++) {
        eroded_image[x][y] = binary[x][y];
        if (binary[x][y] == 255 &&
            (binary[x - 1][y] == 0 || binary[x + 1][y] == 0 ||
             binary[x][y - 1] == 0 || binary[x][y + 1] == 0)) {
          allBlack = 1;
          eroded_image[x][y] = 0;
        } else if (binary[x][y] == 255 &&
                   binary[x + 1][y] + binary[x + 1][y + 1] +
                           binary[x + 1][y - 1] + binary[x][y - 1] +
                           binary[x][y + 1] + binary[x - 1][y] +
                           binary[x - 1][y + 1] + binary[x - 1][y - 1] ==
                       7 * 255) {
          allBlack = 1;
          eroded_image[x][y] = 0;
        }
      }
    }
    capture(eroded_image);
    for (int i = 0; i < BMP_WIDTH; i++) {
      for (int j = 0; j < BMP_HEIGHT; j++) {
        eroded_image[0][j] = 0;
        eroded_image[BMP_WIDTH - 1][j] = 0;
        eroded_image[i][0] = 0;
        eroded_image[i][BMP_HEIGHT - 1] = 0;
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

void red_cross(unsigned char input[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]) {
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      if (input[i][j][0] == 105 && input[i][j][1] == 0 && input[i][j][2] == 0) {
        int cross_size = 9;
        int cross_thickness = 1;
        for (int k = -cross_size; k <= cross_size; k++) {
          input[i + k][j][0] = 255;
          input[i + k][j][1] = 0;
          input[i + k][j][2] = 0;
          for (int l = -cross_size; l <= cross_size; l++) {
            input[i][j + l][0] = 255;
            input[i][j + l][1] = 0;
            input[i][j + l][2] = 0;
            for (int m = -cross_thickness; m <= cross_thickness; m++) {
              input[i + k][j + m][0] = 255;
              input[i + k][j + m][1] = 0;
              input[i + k][j + m][2] = 0;

              input[i + m][j + l][0] = 255;
              input[i + m][j + l][1] = 0;
              input[i + m][j + l][2] = 0;
            }
          }
        }
      }
    }
  }
}

// Main function
int main(int argc, char **argv) {
  clock_t start, end;
  double cpu_time_used;
  start = clock();
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
  greyscale(input_image, bitmap2D_1, bitmap2D_3);
  printf("Converted into grayscale\n");

  // Converts greyscale image to binary image
  threshold(bitmap2D_1, bitmap2D_2, bitmap2D_3);
  printf("Converted into binary \n");

  // Compute the distance transform
  watershed_segmentation(bitmap2D_2);
  printf("Watershed segmentation on binary \n");

  // // Perform watershed segmentation
  // watershedSegmentation(bitmap2D_3);
  // printf("wtaershed segmentation on binary \n");

  // erodes image
  erode(bitmap2D_1, bitmap2D_3);
  printf("Eroded image \n");

  // Prints red cross on original image
  red_cross(input_image);
  printf("Printed red cross' \n");

  // Save image to file
  write_bitmap(input_image, argv[2]);

  /* The code that has to be measured. */
  end = clock();
  cpu_time_used = end - start;
  printf("Total time: %f ms\n", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);

  printf("Done!\n");
  printf("Found %i\n", Captured_spots);
  return 0;
}
