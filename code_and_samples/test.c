


void capture(unsigned char detect_spots[BMP_WIDTH][BMP_HEIGTH]) {
    for (int i = 0; i <= BMP_WIDTH - capture_area; i++) {
        for (int j = 0; j <= BMP_HEIGTH - capture_area; j++) {
            // Hvis vi finder en hvid pixel
            if (detect_spots[i][j] == 255) {
                
                int onEdge = (i == 0 || i == BMP_WIDTH - 1 || j == 0 || j == BMP_HEIGTH - 1) 
                            || (detect_spots[i-1][j] == 0 || detect_spots[i+1][j] == 0 
                            || detect_spots[i][j-1] == 0 || detect_spots[i][j+1] == 0);

                if (onEdge) {
                    int lock = 1;
                    for (int k = 0; k < capture_area && lock; k++) {
                        for (int l = 0; l < capture_area && lock; l++) {
                            if (detect_spots[i + 0][j + l] == 255 ||
                                detect_spots[i + capture_area - 1][j + l] == 255 ||
                                detect_spots[i + k][j + 0] == 255 ||
                                detect_spots[i + k][j + capture_area - 1] == 255) {
                                lock = 0;
                                break;
                            }
                        }
                    }
                    if (lock) {
                        capture_part_2(i, j, detect_spots);
                        // Spring frem med capture_area for at undgå unødvendige gentjekker
                        j += capture_area;
                    }
                }
            }
        }
    }
}


