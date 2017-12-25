#include <iostream>
#include "console.h"
#include "simpio.h"
#include "strlib.h"
#include "set.h"
#include "filelib.h"
#include "vector.h"
#include "queue.h"
#include "lexicon.h"
using namespace std;

void findWordLadder(Lexicon&, string&, string&);

int main() {
    cout << "Welcome to CS 106B/X Word Ladder!" << endl;
    cout << "Please give me two English words, and I will convert the" << endl;
    cout << "first into the second by modifying one letter at a time." << endl;
    cout << endl;

    string filename;
    while (true) {
        filename = getLine("Dictionary file name:");
        if (!isFile(filename))
            cout << "Unable to open that file.  Try again." << endl;
        else
            break;
    }

    Lexicon dictionary(filename);

    while (true) {
        cout << endl;

        string word1, word2;
        word1 = getLine("Word 1 (or Enter to exit):");
        if (word1.size() == 0)
            break;
        word2 = getLine("Word 2 (or Enter to exit):");
        if (word2.size() == 0)
            break;

        if (word1.size() != word2.size()) {
            cout << "The two words must be the same length." << endl;
            continue;
        }

        word1 = toLowerCase(word1);
        word2 = toLowerCase(word2);

        if (word1 == word2) {
            cout << "The two words must be different." << endl;
            continue;
        }
        if (!dictionary.contains(word1) || !dictionary.contains(word2)) {
            cout << "The two words must be found in the dictionary." << endl;
            continue;
        }

        findWordLadder(dictionary, word1, word2);
    }

    cout << "Have a nice day." << endl;
    return 0;
}

void findWordLadder(Lexicon &dictionary, string &word1, string &word2) {
    Queue<Vector<string>> ladders;
    Set<string> usedWords;

    Vector<string> init_ladder;
    init_ladder.add(word2);
    usedWords.add(word2);
    ladders.enqueue(init_ladder);

    bool word1Found = false;
    while (!ladders.isEmpty() && !word1Found) {
        Vector<string> ladder = ladders.dequeue();
        string topWord = ladder[ladder.size() - 1];
        for (int i = 0; i < topWord.size(); i++) {
            for (char j = 'a'; j <= 'z'; j++) {
                string permutation(topWord);
                permutation[i] = j;
                if (permutation == word1) {
                    cout << "A ladder from " << word2 << " back to " << word1 << ": " << endl;
                    for (auto& word : ladder)
                        cout << word << " ";
                    cout << permutation << endl;

                    j = 'z';
                    i = topWord.size();
                    word1Found = true;
                }
                else if (dictionary.contains(permutation) && !usedWords.contains(permutation)) {
                    Vector<string> newLadder = ladder.subList(0, ladder.size());
                    newLadder.add(permutation);
                    usedWords.add(permutation);
                    ladders.enqueue(newLadder);
                }
            }
        }
    }

    if (!word1Found) {
        cout << "No word ladder found from " << word2 << " back to " << word1 << "." << endl;
    }
}
