#include <iostream>
#include <fstream>
#include "console.h"
#include "map.h"
#include "simpio.h"
#include "random.h"
#include "vector.h"
#include "filelib.h"
using namespace std;

int main() {
    cout << "Welcome to CS 106B/X Random Writer ('N-Grams')!" << endl;
    cout << "This program generates random text based on a document." << endl;
    cout << "Give me an input file and an 'N' value for groups" << endl;
    cout << "of words, and I'll create random text for you." << endl;
    cout << endl;

    string filename;
    while (true) {
        filename = getLine("Input file name?");
        if (!isFile(filename))
            cout << "Unable to open that file.  Try again." << endl;
        else
            break;
    }

    ifstream text(filename);

    string word;
    Vector<string> words;
    while (text >> word)
        words.add(word);

    int n;
    while (true) {
        n = getInteger("Value of N?");
        if (n < 2)
            cout << "N must be 2 or greater." << endl;
        else
            break;
    }

    Map<Vector<string>, Vector<string>> ngrams;
    Vector<string> window = words.subList(0, n);
    for (int i = 0; i < words.size(); i++) {
        int j = (i + n + 1) % words.size();
        ngrams[window].add(words[j]);
        window.remove(0);
        window.add(words[j]);
    }

    while (true) {
        cout << endl;
        int m = getInteger("# of random words to generate (0 to quit)?");
        if (m == 0)
            break;
        else if (m < n) {
            cout << "Must be at least " << n << " words." << endl;
            continue;
        }

        cout << "... ";

        int start = randomInteger(0, ngrams.size() - 1);
        Vector<string> prefix = ngrams.keys()[start];
        for(auto& word : prefix){
            cout << word << " ";
        }

        for (int i = 0; i < m - n; i++) {
            int r = randomInteger(0, ngrams[prefix].size() - 1);
            string next_word = ngrams[prefix][r];
            cout << next_word << " ";
            prefix.remove(0);
            prefix.add(next_word);
        }

        cout << "..." << endl;
    }

    cout << "Exiting." << endl;
    return 0;
}
