#include "tilelist.h"

#define YELLOW "#ffff00"

TileList::TileList() {
    front = NULL;
    back = NULL;
}

TileList::~TileList() {
    deleteAllTiles();
}

void TileList::debug() {}

void TileList::addBack(int x, int y, int width, int height, string color) {
    TileNode *newTile = new TileNode(x, y, width, height, color, back, NULL);
    if (back != NULL)
        back->next = newTile;
    else
        front = newTile;
    back = newTile;
}

void TileList::addFront(int x, int y, int width, int height, string color) {
    TileNode *newTile = new TileNode(x, y, width, height, color, NULL, front);
    if (front != NULL)
        front->prev = newTile;
    else
        back = newTile;
    front = newTile;
}

void TileList::clear() {
    deleteAllTiles();
}

TileNode* tileAtPosition(TileNode *front, int x, int y) {
    TileNode *tile = front;
    while (tile != NULL && !tile->contains(x, y))
        tile = tile->next;
    return tile;
}

void TileList::drawAll(GWindow& window) {
    TileNode *tile = back;
    while (tile != NULL) {
        tile->draw(window);
        tile = tile->prev;
    }
}

void TileList::highlight(int x, int y) {
    TileNode *tile = tileAtPosition(front, x, y);
    tile->color = YELLOW;
}

void TileList::lower(int x, int y) {
    TileNode *tile = tileAtPosition(front, x, y);
    if (tile != NULL && tile != back) {
        if (tile == front)
            front = front->next;

        if (tile->prev != NULL)
            tile->prev->next = tile->next;
        if (tile->next != NULL)
            tile->next->prev = tile->prev;

        back->next = tile;
        tile->prev = back;
        tile->next = NULL;
        back = tile;
    }
}

void TileList::raise(int x, int y) {
    TileNode *tile = tileAtPosition(front, x, y);
    if (tile != NULL) {
        if (tile == back)
            back = back->prev;

        if (tile->prev != NULL)
            tile->prev->next = tile->next;
        if (tile->next != NULL)
            tile->next->prev = tile->prev;

        front->prev = tile;
        tile->prev = NULL;
        tile->next = front;
        front = tile;
    }
}

void TileList::remove(int x, int y) {
    TileNode *tile = tileAtPosition(front, x, y);
    if (tile != NULL)
        deleteTile(tile);
}

void TileList::removeAll(int x, int y) {
    TileNode *tile = front;
    while (tile != NULL) {
        TileNode *toDelete = tile;
        tile = tile->next;
        if (toDelete->contains(x, y))
            deleteTile(toDelete);
    }
}

void TileList::deleteAllTiles() {
    TileNode *tile = front;
    while (tile != NULL) {
        TileNode *toDelete = tile;
        tile = tile->next;
        deleteTile(toDelete);
    }
}

void TileList::deleteTile(TileNode *tile) {
    if (tile == front)
        front = front->next;
    else if (tile == back)
        back = back->prev;

    if (tile->prev != NULL)
        tile->prev->next = tile->next;
    if (tile->next != NULL)
        tile->next->prev = tile->prev;

    if (front == back)
        front = back = NULL;
    delete tile;
}
