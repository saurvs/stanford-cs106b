#ifndef _tilelist_h
#define _tilelist_h

#include <string>
#include "gwindow.h"
#include "tilenode.h"
using namespace std;

class TileList {
public:
    TileList();
    ~TileList();
    void addFront(int x, int y, int width, int height, string color);
    void addBack(int x, int y, int width, int height, string color);
    void clear();
    void debug();
    void drawAll(GWindow& window);

    void highlight(int x, int y);
    void lower(int x, int y);
    void raise(int x, int y);
    void remove(int x, int y);
    void removeAll(int x, int y);

private:
    TileNode *front;
    TileNode *back;

    void deleteTile(TileNode *tile);
    void deleteAllTiles();
};

#endif // _tilelist_h
