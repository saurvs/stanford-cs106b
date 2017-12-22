#include <iostream>
#include "console.h"
#include "simpio.h"
#include "gbufferedimage.h"
#include "grid.h"
#include "gwindow.h"
#include "fauxtoshop-provided.h"   // instructor-provided code
#include "strlib.h"

#define GREEN 0x00FF00

using namespace std;

int rgbDiff(int, int);
void applyScatter(GBufferedImage&);
void applyEdgeDetection(GBufferedImage&);
void applyGreenScreen(GBufferedImage&);
void performCompare(const GWindow&, const GBufferedImage&);
void applyGaussianBlur(GBufferedImage &);

int main() {
    cout << "Welcome to Fauxtoshop!" << endl;

    // basic setup of Graphics Window
    GWindow gw;
    gw.setTitle("Fauxtoshop");
    gw.setExitOnClose(true);
    gw.setVisible(true);

    // uncomment this if you want the same random numbers on each run
    // fakeRandomNumberGenerator();

    string filename;
    GBufferedImage img;

  fauxtoshop_start:
    do {
        cout << "Enter name of image file to open (or blank to quit): ";
        getline(cin, filename);
        if (filename.empty())
            goto fauxtoshop_exit;
    } while(!openImageFromFilename(img, filename));

    gw.setCanvasSize(img.getWidth(), img.getHeight());
    gw.add(&img, 0, 0);

    int choice;
    do {
        cout << "Which image filter would you like to apply?" << endl;
        cout << "\t1 - Scatter" << endl;
        cout << "\t2 - Edge detection" << endl;
        cout << "\t3 - \"Green screen\" with another image" << endl;
        cout << "\t4 - Compare with another image" << endl;
        cout << "\t5 - Gaussian Blur" << endl;
        choice = getInteger("Your choice: ");
    } while (choice < 1 || choice > 5);

    switch (choice) {
        case 1:
            applyScatter(img);
            break;
        case 2:
            applyEdgeDetection(img);
            break;
        case 3:
            applyGreenScreen(img);
            break;
        case 4:
            performCompare(gw, img);

            cout << endl;
            goto fauxtoshop_start;
        case 5:
            applyGaussianBlur(img);
            break;
    }

    do {
        cout << "Enter filename to save image (or blank to skip saving): ";
        getline(cin, filename);
        if (filename.empty())
            break;
    } while(!saveImageToFilename(img, filename));

    cout << endl;
    goto fauxtoshop_start;

  fauxtoshop_exit:
    cout << "Exiting." << endl;
    return 0;
}

int rgbDiff(int c1, int c2) {
    int r1, g1, b1;
    GBufferedImage::getRedGreenBlue(c1, r1, g1, b1);

    int r2, g2, b2;
    GBufferedImage::getRedGreenBlue(c2, r2, g2, b2);

    int rd = abs(r1 - r2);
    int gd = abs(g1 - g2);
    int bd = abs(b1 - b2);

    return max(rd, max(gd, bd));
}

void applyScatter(GBufferedImage &img) {
    int d;
    do {
        d = getInteger("Enter degree of scatter [1-100]: ");
    }
    while (d < 1 || d > 100);

    Grid <int> original = img.toGrid();
    Grid <int> scattered (original.height(), original.width());

    for (int i = 0; i < original.width(); i++) {
        for (int j = 0; j < original.height(); j++) {
            int x, y;

            do {
                x = i + randomInteger(-d, d);
                y = j + randomInteger(-d, d);
            }
            while (!original.inBounds(y, x));

            scattered[j][i] = original[y][x];
        }
    }

    img.fromGrid(scattered);
}

void applyEdgeDetection(GBufferedImage &img) {
    int threshold;
    do {
        threshold = getInteger("Enter threshold for edge detection: ");
    }
    while (threshold < 0);

    Grid <int> original = img.toGrid();
    Grid <int> edge (original.height(), original.width());

    for (int i = 0; i < original.width(); i++) {
        for (int j = 0; j < original.height(); j++) {
            int max_diff = 0;
            for (int k = -1; k < 2; k++) {
                for (int l = -1; l < 2; l++) {
                    int n_col = i + k;
                    int n_row = j + l;
                    if (original.inBounds(n_row, n_col)) {
                        int neighbour = original[n_row][n_col];
                        max_diff = max(max_diff, rgbDiff(original[j][i], neighbour));
                    }
                }
            }

            if (max_diff > threshold)
                edge[j][i] = BLACK;
            else
                edge[j][i] = WHITE;
        }
    }

    img.fromGrid(edge);
}

void applyGreenScreen(GBufferedImage &img) {
    cout << "Now choose another file to add to your background image." << endl;
    string filename;
    GBufferedImage foreground;
    do {
        cout << "Enter name of image file to open: ";
        getline(cin, filename);
    } while(!openImageFromFilename(foreground, filename));

    int threshold;
    do {
        threshold = getInteger("Now choose a tolerance threshold [0-100]: ");
    }
    while (threshold < 0 || threshold > 100);

    cout << "Enter location to place the image as \"(row,col)\" (or blank to use mouse): ";
    string location;
    getline(cin, location);

    int foreRow, foreCol;
    if (location.empty()) {
        cout << "Now click the background image to place new image." << endl;
        getMouseClickLocation(foreRow, foreCol);
        cout << "You chose (" << foreRow << "," << foreCol << ")" << endl;
    } else {
        vector<string> v = stringSplit(location, ",");
        v[0].erase(0, 1);
        v[1].erase(v[1].size() - 1, 1);
        foreRow = stringToInteger(v[0]);
        foreCol = stringToInteger(v[1]);
    }

    Grid <int> backGrid = img.toGrid();
    Grid <int> foreGrid = foreground.toGrid();
    Grid <int> newGrid (backGrid.height(), backGrid.width());

    for (int i = 0; i < backGrid.width(); i++) {
        for (int j = 0; j < backGrid.height(); j++) {
            int k = i - foreCol;
            int l = j - foreRow;
            if (foreGrid.inBounds(l, k)) {
                if (rgbDiff(foreGrid[l][k], GREEN) > threshold)
                    newGrid[j][i] = foreGrid[l][k];
                else
                    newGrid[j][i] = backGrid[j][i];
            } else
                newGrid[j][i] = backGrid[j][i];
        }
    }

    img.fromGrid(newGrid);
}

void performCompare(const GWindow &gw, const GBufferedImage &img1) {
    cout << "Now choose another file to compare to." << endl;
    string filename;
    GBufferedImage img2;
    do {
        cout << "Enter name of image file to open: ";
        getline(cin, filename);
    } while(!openImageFromFilename(img2, filename));

    int diff = img1.countDiffPixels(img2);
    if (diff > 0) {
        cout << "These images differ in " << diff << " pixel locations!" << endl;
        showDiffWindow(gw, filename);
    }
    else
        cout << "These images are the same!" << endl;
}

void applyGaussianBlur(GBufferedImage &img) {
    int radius;
    do {
        radius = getInteger("Enter radius [1-100]: ");
    }
    while (radius < 1 || radius > 100);

    Grid <int> grid1 = img.toGrid();
    Grid <int> grid2 (grid1.height(), grid1.width());
    Vector<double> kernel = gaussKernelForRadius(radius);

    // horizontal convolution: grid1 -> grid2
    for (int i = 0; i < grid1.width(); i++) {
        for (int j = 0; j < grid1.height(); j++) {
            double r = 0, g = 0, b = 0;

            for (int k = -radius; k <= radius; k++) {
                int r1, g1, b1;

                if (i + k < 0)
                    GBufferedImage::getRedGreenBlue(grid1[j][0], r1, g1, b1);
                else if (i + k >= grid1.width())
                    GBufferedImage::getRedGreenBlue(grid1[j][grid1.width() - 1], r1, g1, b1);
                else
                    GBufferedImage::getRedGreenBlue(grid1[j][i + k], r1, g1, b1);

                r += kernel[k + radius] * r1;
                g += kernel[k + radius] * g1;
                b += kernel[k + radius] * b1;
            }

            grid2[j][i] = GBufferedImage::createRgbPixel(r, g, b);
        }
    }

    // verticle convolution: grid2 -> grid1
    for (int i = 0; i < grid2.width(); i++) {
        for (int j = 0; j < grid2.height(); j++) {
            double r = 0, g = 0, b = 0;

            for (int k = -radius; k <= radius; k++) {
                int r1, g1, b1;

                if (j + k < 0)
                    GBufferedImage::getRedGreenBlue(grid2[0][i], r1, g1, b1);
                else if (j + k >= grid2.height())
                    GBufferedImage::getRedGreenBlue(grid2[grid2.height() - 1][i], r1, g1, b1);
                else
                    GBufferedImage::getRedGreenBlue(grid2[j + k][i], r1, g1, b1);

                r += kernel[k + radius] * r1;
                g += kernel[k + radius] * g1;
                b += kernel[k + radius] * b1;
            }

            grid1[j][i] = GBufferedImage::createRgbPixel(r, g, b);
        }
    }

    img.fromGrid(grid1);
}
