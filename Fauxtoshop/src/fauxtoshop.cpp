#include <iostream>
#include <iomanip>
#include <limits>
#include "console.h"
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
        cout << "Your choice: ";
        cin >> choice;
    } while (choice < 1 || choice > 4);

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
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
        cout << "Enter degree of scatter [1-100]: ";
        cin >> d;
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
        cout << "Enter threshold for edge detection: ";
        cin >> threshold;
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

                    if (original.inBounds(n_row, n_col))
                        max_diff = max(max_diff, rgbDiff(original[j][i], original[n_row][n_col]));
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
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    do {
        cout << "Enter name of image file to open: ";
        getline(cin, filename);
    } while(!openImageFromFilename(foreground, filename));

    int threshold;
    do {
        cout << "Now choose a tolerance threshold [0-100]: ";
        cin >> threshold;
    }
    while (threshold < 0 || threshold > 100);

    cout << "Enter location to place the image as \"(row,col)\" (or blank to use mouse): ";
    string location;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
