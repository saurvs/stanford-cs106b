#include <string>
#include <thread>
#include <chrono>
#include "grid.h"
#include "lexicon.h"
#include "set.h"
#include "vector.h"
#include "shuffle.h"
#include "random.h"
#include "bogglegui.h"

using namespace std;

bool singleWordSearch(const Grid<char>&, const string&, const GPoint, Set<GPoint>&);
void multipleWordSearch(const Grid<char>&, const Lexicon&, Set<string>&, string, GPoint,
                        Set<GPoint>&);
int scoreForWordSize(int);

Grid<char> generateRandomBoard(int size, const Vector<std::string>& letterCubes) {
    Vector<string> randomOrderedCubes;
    randomOrderedCubes.addAll(letterCubes);
    shuffle(randomOrderedCubes);

    Grid<char> board(size, size);
    int i = 0, j = i;
    for (auto& cube : randomOrderedCubes) {
        int r = randomInteger(0, cube.size() - 1);
        board[i][j] = cube[r];
        if (i == size - 1)
            j++;
        i = (i + 1) % size;
    }
    return board;
}

bool humanWordSearch(const Grid<char>& board, const Lexicon& dictionary, const string& word) {
    if (word.size() < BoggleGUI::MIN_WORD_LENGTH || !dictionary.contains(word))
        return false;

    BoggleGUI::clearHighlighting();
    for (int x = 0; x < board.numRows(); x++) {
        for (int y = 0; y < board.numCols(); y++) {
            Set<GPoint> visited;
            if (singleWordSearch(board, word, GPoint(x, y), visited)) {
                BoggleGUI::scorePointsHuman(scoreForWordSize(word.size()));
                return true;
            }
        }
    }
    return false;
}

bool singleWordSearch(const Grid<char>& board, const string& word, const GPoint startPoint,
                      Set<GPoint>& visited) {
    visited.add(startPoint);

    this_thread::sleep_for(chrono::milliseconds(100));
    BoggleGUI::setHighlighted(startPoint.getX(), startPoint.getY(), true);

    if (board[startPoint.getX()][startPoint.getY()] != word[0]) {
        BoggleGUI::setHighlighted(startPoint.getX(), startPoint.getY(), false);
        visited.remove(startPoint);
        return false;
    }
    else if (word.size() == 1)
        return true;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int x = startPoint.getX() + i;
            int y = startPoint.getY() + j;
            GPoint next = GPoint(x, y);
            if (board.inBounds(x, y)
                    && !visited.contains(next)
                    && singleWordSearch(board, word.substr(1, word.size() - 1), next, visited))
                return true;
        }
    }

    this_thread::sleep_for(chrono::milliseconds(100));
    BoggleGUI::setHighlighted(startPoint.getX(), startPoint.getY(), false);
    visited.remove(startPoint);

    return false;
}

Set<string> computerWordSearch(const Grid<char>& board, const Lexicon& dictionary,
                               const Set<string>& humanWords) {
    Set<string> words;
    for (int x = 0; x < board.numRows(); x++) {
        for (int y = 0; y < board.numCols(); y++) {
            string prefix;
            prefix.push_back(board[x][y]);

            Set<string> found;
            Set<GPoint> visited;
            multipleWordSearch(board, dictionary, found, prefix, GPoint(x, y), visited);

            for (auto& word : found)
                if (!humanWords.contains(word) && !words.contains(word))
                    words.add(word);
        }
    }

    int score = 0;
    for (auto& word : words)
        score += scoreForWordSize(word.size());
    BoggleGUI::scorePointsComputer(score);

    return words;
}

void multipleWordSearch(const Grid<char>& board, const Lexicon& dictionary, Set<string>& found,
                        string prefix, GPoint startPoint, Set<GPoint>& visited) {
    if (dictionary.containsPrefix(prefix)) {
        visited.add(startPoint);
        this_thread::sleep_for(chrono::milliseconds(100));
        BoggleGUI::setHighlighted(startPoint.getX(), startPoint.getY(), true);

        if (dictionary.contains(prefix) && prefix.size() >= BoggleGUI::MIN_WORD_LENGTH)
            found.add(prefix);

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int x = startPoint.getX() + i;
                int y = startPoint.getY() + j;
                GPoint neighbour = GPoint(x, y);
                if (board.inBounds(x, y) && !visited.contains(neighbour)) {
                    string next_prefix = prefix;
                    next_prefix.push_back(board[x][y]);
                    multipleWordSearch(board, dictionary, found, next_prefix, neighbour, visited);
                }
            }
        }

        this_thread::sleep_for(chrono::milliseconds(100));
        BoggleGUI::setHighlighted(startPoint.getX(), startPoint.getY(), false);
        visited.remove(startPoint);
    }
}

int scoreForWordSize(int wordSize) {
    if (wordSize <= 4)
        return 1;
    else if (wordSize == 5)
        return 2;
    else if (wordSize == 6)
        return 3;
    else if (wordSize == 7)
        return 5;
    else
        return 11;
}
