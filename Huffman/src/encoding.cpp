#include "encoding.h"
#include "pqueue.h"
#include "bitstream.h"
#include "filelib.h"

void buildEncodingMap(HuffmanNode* encodingTree, Map<int, string>& encodingMap, string code = "");
void writeBits(obitstream& output, string bits);

Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;

    char c;
    while (input.get(c)) {
        if (!freqTable.containsKey(c))
            freqTable[c] = 0;
        freqTable[c]++;
    }

    freqTable.put(PSEUDO_EOF, 1);
    return freqTable;
}

HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    PriorityQueue<HuffmanNode*> pq;

    for (auto& c : freqTable.keys()) {
        int freq = freqTable[c];
        HuffmanNode *node = new HuffmanNode(c, freq);
        pq.add(node, freq);
    }

    HuffmanNode *root = NULL;

    while (true) {
        if (pq.isEmpty())
            break;
        HuffmanNode *n1 = pq.dequeue();

        if (pq.isEmpty()) {
            root = n1;
            break;
        }
        HuffmanNode *n2 = pq.dequeue();

        int freq = n1->count + n2->count;
        HuffmanNode *n3 = new HuffmanNode(NOT_A_CHAR, freq, n1, n2);
        pq.add(n3, freq);
    }

    return root;
}

Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    buildEncodingMap(encodingTree, encodingMap);
    return encodingMap;
}

void buildEncodingMap(HuffmanNode* encodingTree, Map<int, string>& encodingMap, string code) {
    if (encodingTree->character != NOT_A_CHAR) {
        encodingMap[encodingTree->character] = code;
        return;
    }

    string zero = code;
    zero.push_back('0');
    buildEncodingMap(encodingTree->zero, encodingMap, zero);

    string one = code;
    one.push_back('1');
    buildEncodingMap(encodingTree->one, encodingMap, one);
}

void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    char c;
    while (input.get(c))
        writeBits(output, encodingMap[c]);
    writeBits(output, encodingMap[PSEUDO_EOF]);
}

void writeBits(obitstream& output, string bits) {
    for (auto& bit : bits)
        output.writeBit(bit - '0');
}

void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    HuffmanNode *current = encodingTree;

    int bit = input.readBit();
    while (bit != -1 && current->character != PSEUDO_EOF) {
        if (current->character != NOT_A_CHAR) {
            output << (char)current->character;
            current = encodingTree;
        }

        if (bit == 0)
            current = current->zero;
        else
            current = current->one;

        bit = input.readBit();
    }
}

void compress(istream& input, obitstream& output) {
    Map<int, int> freqTable = buildFrequencyTable(input);
    rewindStream(input);

    output << freqTable;

    HuffmanNode *encodingTree = buildEncodingTree(freqTable);
    Map<int, string> encodingMap = buildEncodingMap(encodingTree);

    encodeData(input, encodingMap, output);
}

void decompress(ibitstream& input, ostream& output) {
    Map<int, int> freqTable;
    input >> freqTable;

    HuffmanNode *encodingTree = buildEncodingTree(freqTable);
    decodeData(input, encodingTree, output);
}

void freeTree(HuffmanNode* node) {
    if (!node->isLeaf()) {
        freeTree(node->zero);
        freeTree(node->one);
    }
    delete node;
}
