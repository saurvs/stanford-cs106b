#include <iostream>
#include <iomanip>
#include "console.h"
#include "gbufferedimage.h"
#include "grid.h"
#include "gwindow.h"
#include "fauxtoshop-provided.h"   // instructor-provided code
using namespace std;

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

  fauxtoshop_exit:
    cout << "Exiting." << endl;
    return 0;
}
