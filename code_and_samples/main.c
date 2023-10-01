// To compile (linux/mac): gcc cbmp.c main.c -o main.out -std=c99
// To run (linux/mac): ./main.out samples/easy/2EASY.bmp output.bmp

// To compile (win): gcc cbmp.c main.c -o main.exe -std=c99
// To run (win): .\main.exe example.bmp output.bmp

#include "cbmp.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global variables
int capture_area = 14;
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char bitmap2D_1[BMP_WIDTH][BMP_HEIGHT];
unsigned char bitmap2D_2[BMP_WIDTH][BMP_HEIGHT];
int Captured_spots = 0;

// Converts the 3D bitmap to greyscale, ie. one color per pixel.
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

// creates a histogram from af bitmap image to be used in Otsu's method
void calculateHistogram(unsigned char bitmap[BMP_WIDTH][BMP_HEIGHT],
                        int histogram[256]) {
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      histogram[bitmap[i][j]]++;
    }
  }
}

// Calculates the cumulative distribution function for the given histogram
void calculateCDF(int histogram[256], double cdf[256], int totalPixelCount) {
  cdf[0] = (double)histogram[0] / totalPixelCount;
  for (int i = 1; i < 256; i++) {
    cdf[i] = cdf[i - 1] + (double)histogram[i] / totalPixelCount;
  }
}

// Calculates the mean value of the given bitmap images
float calculateMean(unsigned char bitmap[BMP_WIDTH][BMP_HEIGHT],
                    int totalPixelCount) {
  float mean = 0.0;
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      mean += (float)bitmap[i][j] / totalPixelCount;
    }
  }
  return mean;
}

// Calculates the Threshold of a given bitmap image through Otsu's method
int calculateOtsuThreshold(unsigned char bitmap[BMP_WIDTH][BMP_HEIGHT]) {
  int histogram[256] = {0};
  calculateHistogram(bitmap, histogram);

  int totalPixelCount = BMP_WIDTH * BMP_HEIGHT;

  double cdf[256];
  calculateCDF(histogram, cdf, totalPixelCount);

  float mean = calculateMean(bitmap, totalPixelCount);

  float maxVariance = 0.0;
  int threshold = 0;

  // Calculates the threshold based on the highest variance
  for (int i = 0; i < 256; i++) {
    float p0 = cdf[i];
    float p1 = 1.0 - cdf[i];
    float mu0 = 0.0;
    float mu1 = 0.0;

    if (p0 > 0.0) {
      mu0 = (mean - cdf[i - 1]) / p0;
    }
    if (p1 > 0.0) {
      mu1 = (mean - cdf[i - 1]) / p1;
    }

    float variance = p0 * p1 * (mu0 - mu1) * (mu0 - mu1);
    if (variance > maxVariance) {
      maxVariance = variance;
      threshold = i;
    }
  }

  return threshold;
}

// Applys the calculated threshold on the greyscaled image, to convert it to a
// binary 2D bitmap
void threshold(unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT],
               unsigned char binary_image[BMP_WIDTH][BMP_HEIGHT],
               unsigned char Otsu_image[BMP_WIDTH][BMP_HEIGHT]) {
  int thresholdInt = calculateOtsuThreshold(Otsu_image);
  free(Otsu_image);
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGHT; y++) {
      if (greyscale_image[x][y] < thresholdInt) {
        binary_image[x][y] = 0;
      } else {
        binary_image[x][y] = 255;
      }
    }
  }

  // Creates a copy of the binary 2D bitmap and overwrites the greyscaled image
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGHT; y++) {
      greyscale_image[x][y] = binary_image[x][y];
    }
  }
}

// Calculates the squared Euclidean distance
double squaredEuclideanDistance(int x1, int y1, int x2, int y2) {
  int dx = x1 - x2;
  int dy = y1 - y2;
  return dx * dx + dy * dy;
}

// An attept at creating a watershed segmentation algorithm. Doesn't work, but
// still improves performance of program.
void watershed_segmentation(unsigned char binaryImage[BMP_WIDTH][BMP_HEIGHT]) {
  int maxDistance = BMP_WIDTH * BMP_HEIGHT;
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      if (binaryImage[i][j] == 255) {
        unsigned char min_neighbor_distance = maxDistance;

        // Check neighbors
        for (int x = -1; x <= 1; x++) {
          for (int y = -1; y <= 1; y++) {
            int nx = i + x;
            int ny = j + y;
            if ((nx >= 0 && nx < BMP_WIDTH) && (ny >= 0 && ny < BMP_HEIGHT)) {
              if (binaryImage[nx][ny] == 0) {
                unsigned int dist = squaredEuclideanDistance(i, j, nx, ny);
                if (dist < min_neighbor_distance) {
                  min_neighbor_distance = (unsigned char)dist;
                }
              }
            }
          }
        }
        // In theory this would be a ridge which would cut through joint cells
        if (min_neighbor_distance < 2) {
          bitmap2D_1[i][j] = 0;
        }
      }
    }
  }
}

// Part 2 of the capture process. Checks whether or not there exists a white
// pixel within the 12x12 area since the border already has been tjekked
void capture_part_2(int x, int y,
                    unsigned char detect_spots[BMP_WIDTH][BMP_HEIGHT]) {
  for (int m = 1; m < capture_area - 1; m++) {
    for (int n = 1; n < capture_area - 1; n++) {
      if (detect_spots[x + m][y + n] == 255) {
        Captured_spots++;
        for (int o = 1; o < capture_area - 1; o++) {
          for (int p = 1; p < capture_area - 1; p++) {
            bitmap2D_2[x + o][y + p] = 0;
            detect_spots[x + o][y + p] = 0;
            // assigns a specific value to the middle of a cell to be used
            // later. Can choose the value for certain since the input picture
            // doesn't have a red pixel.
            input_image[x + 6][y + 6][0] = 105;
            input_image[x + 6][y + 6][1] = 0;
            input_image[x + 6][y + 6][2] = 0;
          }
        }
      }
    }
  }
}

// Part 1 of the capture process. Checks the safe area around the 12x12 area for
// white pixels if none it proceeds to part 2
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
        j += capture_area - 2;
      }
    }
  }
}

// Erodes the binary 2D bitmap
void erode(unsigned char binary[BMP_WIDTH][BMP_HEIGHT],
           unsigned char eroded_image[BMP_WIDTH][BMP_HEIGHT]) {
  // Used to know how many times the picture needs to be eroded
  int allBlack = 1;
  while (allBlack) {
    allBlack = 0;
    for (int x = 0; x < BMP_WIDTH; x++) {
      for (int y = 0; y < BMP_HEIGHT; y++) {
        // Gets the latest eroded step into the processed bitmap
        eroded_image[x][y] = binary[x][y];

        // The original erode algorithm, check the horizontal and vertical
        // neighbours
        if (binary[x][y] == 255 &&
            (binary[x - 1][y] == 0 || binary[x + 1][y] == 0 ||
             binary[x][y - 1] == 0 || binary[x][y + 1] == 0)) {
          allBlack = 1;
          eroded_image[x][y] = 0;

          // The improved erode algorithm, checks the diagonal neighbours
        } else if (binary[x][y] == 255 &&
                   binary[x + 1][y + 1] + binary[x + 1][y - 1] +
                           binary[x - 1][y + 1] + binary[x - 1][y - 1] ==
                       3 * 255) {
          allBlack = 1;
          eroded_image[x][y] = 0;
        }
      }
    }
    // tries to capture cells on the eroded image
    capture(eroded_image);

    // Copies the eroded image over to the processed image, with border control.
    for (int i = 0; i < BMP_WIDTH; i++) {
      for (int j = 0; j < BMP_HEIGHT; j++) {
        eroded_image[0][j] = 0;
        eroded_image[BMP_WIDTH - 1][j] = 0;
        eroded_image[i][0] = 0;
        eroded_image[i][BMP_HEIGHT - 1] = 0;
        binary[i][j] = eroded_image[i][j];
      }
    }
  }
}

// Draws the red cross on the original image
void red_cross(unsigned char input[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS]) {
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      if (input[i][j][0] == 105 && input[i][j][1] == 0 && input[i][j][2] == 0) {
        printf("cell detected at (%i,%i)\n", i, j);
        int cross_size = 9;
        int cross_thickness = 1;
        // Draws the vertical line of the cross
        for (int k = -cross_size; k <= cross_size; k++) {
          input[i + k][j][0] = 255;
          input[i + k][j][1] = 0;
          input[i + k][j][2] = 0;
          // Draws the horizontal line of the cross
          for (int l = -cross_size; l <= cross_size; l++) {
            input[i][j + l][0] = 255;
            input[i][j + l][1] = 0;
            input[i][j + l][2] = 0;
            // makes the cross thicker and easier to see
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
  // Starts a clock to measure execution speed
  clock_t start, end;
  double cpu_time_used;
  start = clock();
  // Checking that 2 arguments are passed
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <output file path> <output file path>\n",
            argv[0]);
    exit(1);
  }

  // Load image from file
  read_bitmap(argv[1], input_image);

  // Creates a dynamic array
  unsigned char(*bitmap2D_3)[BMP_HEIGHT];
  bitmap2D_3 = (unsigned char(*)[BMP_HEIGHT])malloc(BMP_WIDTH * BMP_HEIGHT *
                                                    sizeof(unsigned char));
  if (bitmap2D_3 == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
  }

  // Run greyscale on input image
  greyscale(input_image, bitmap2D_1, bitmap2D_3);

  // Converts greyscale 3D bitmap to binary 2D bitmap
  threshold(bitmap2D_1, bitmap2D_2, bitmap2D_3);

  // Compute the distance transform and watershed segmentation on the binary 2D bitmap
  watershed_segmentation(bitmap2D_2);

  // Erode the binary 2D bitmap
  erode(bitmap2D_1, bitmap2D_2);

  // Prints red cross on original image
  red_cross(input_image);

  // Save image to file
  write_bitmap(input_image, argv[2]);

  // ends the clock and prints time used and cells capturen
  end = clock();
  cpu_time_used = end - start;
  printf("Total time: %f ms\n", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);
  printf("Cells captured: %i\n", Captured_spots);
  return 0;
}
