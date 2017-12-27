#include "fractals.h"

using namespace std;

#define SQRT_3 1.73205080757

GPoint midGPoint(GPoint &p1, GPoint &p2) {
    return GPoint(
        (p2.getX() + p1.getX())/2,
        (p2.getY() + p1.getY())/2
    );
}

void drawSierpinskiTriangle(GWindow& window, double x, double y, double size, int order) {
    GPoint topLeft = GPoint(x, y);
    GPoint topRight = GPoint(x + size, y);

    GPoint bottomMiddle = midGPoint(topLeft, topRight);
    double height = SQRT_3 * size / 2;
    bottomMiddle = GPoint(bottomMiddle.getX(), bottomMiddle.getY() + height);

    if (order == 1) {
        window.drawLine(topLeft, topRight);
        window.drawLine(topRight, bottomMiddle);
        window.drawLine(bottomMiddle, topLeft);
    } else {
        drawSierpinskiTriangle(window, topLeft.getX(), topLeft.getY(), size/2, order - 1);

        GPoint topMiddle = midGPoint(topLeft, topRight);
        drawSierpinskiTriangle(window, topMiddle.getX(), topMiddle.getY(), size/2, order - 1);

        GPoint leftEdgeMiddle = midGPoint(topLeft, bottomMiddle);
        drawSierpinskiTriangle(window, leftEdgeMiddle.getX(), leftEdgeMiddle.getY(), size/2, order - 1);
    }
}

int floodFill(GWindow& window, int x, int y, int color) {
    if (window.inCanvasBounds(x, y)) {
        int pixelsColored = 0;

        int currentColor = window.getPixel(x, y);
        if(color != currentColor) {
            window.setPixel(x, y, color);
            pixelsColored++;

            if (window.getPixel(x + 1, y) == currentColor)
                pixelsColored += floodFill(window, x + 1, y, color);

            if (window.getPixel(x, y + 1) == currentColor)
                pixelsColored += floodFill(window, x, y + 1, color);

            if (window.getPixel(x - 1, y) == currentColor)
                pixelsColored += floodFill(window, x - 1, y, color);

            if (window.getPixel(x, y - 1) == currentColor)
                pixelsColored += floodFill(window, x, y - 1, color);
        }

        return pixelsColored;
    }
    return 0;
}
