#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "Anchor.h" // Make sure this header is correctly implemented

using namespace std;

class Anchor {
public:
    string pattern;
    int position;
    int nHits, nTries, nMisses;
    bool isActive;

    Anchor(string pat, int pos) : pattern(pat), position(pos), nHits(0), nMisses(0), nTries(0) {}

    float getHitRatio() const {
        return nTries > 0 ? static_cast<float>(nHits) / nTries : 0.0f;
    }

    void recordHit() {
        nHits++;
        nTries++;
    }

    void recordMiss() {
        nMisses++;
        nTries++;
    }

    void deactivate() { isActive = false;}
};

unordered_map<char, int> alphabet;
int totalChars = 0;
int alphabetSize;

unordered_map<string, string> getFlags(int argc, char* argv[]) {
    unordered_map<string, string> flags;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            string flag = arg.substr(2);
            if (i + 1 < argc && argv[i + 1][0] != '-') flags[flag] = argv[++i];
            else flags[flag] = "";
        } else if (arg.substr(0, 1) == "-") {
            string flag = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') flags[flag] = argv[++i];
            else flags[flag] = "";
        }
    }
    return flags;
}

int getAlphabetFrequency() {

    char byte;

    ifstream file("chry.txt");

    if (!file.is_open()) {
        cerr << "Error opening the file.\n";
        return 1;
    }

    while (file.get(byte)) {
        alphabet[byte]++;
        totalChars++;
    }

    // Close the file
    file.close();

    alphabetSize = alphabet.size();

    return 0;
}

float predictProbability(int hits, int misses, float alpha) {
    return (hits + alpha) / (hits + misses + 2 * alpha);
}

int main(int argc, char* argv[]) {
    const size_t MAX_ANCHORS = 4;
    string filename = "chry.txt"; // Default file name
    float threshold = 0.7; // Default threshold to stop a copy model
    float kmerSize = 8; // Default size of sequence to use as anchors
    float alpha = 1; // Default smoothing factor
    vector<Anchor> anchors;
    float nbits = 0, defaultNbits = 0;
    int encodedChars = 0, nonEncodedChars = 0, position = 0;

    auto flags = getFlags(argc, argv);
    if (flags.find("f") != flags.end()) filename = flags["f"];
    if (flags.find("t") != flags.end()) threshold = stof(flags["t"]);
    if (flags.find("a") != flags.end()) alpha = stof(flags["a"]);
    if (flags.find("k") != flags.end()) kmerSize = stof(flags["k"]);

    getAlphabetFrequency(); // This updates the global alphabetSize
    float defaultNbitsPerChar = log2(alphabetSize);

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening the file.\n";
        return 1;
    }

    string window, completeString;
    unordered_map<string, int> hashTable; // Tracks the last position where each k-mer was found
    char byte;
    while (file.get(byte)) {
        defaultNbits += defaultNbitsPerChar;
        if (window.size() >= kmerSize) window.erase(0, 1);
        window.push_back(byte);

        bool anyActiveAnchor = false;
        bool foundMatchingAnchor = false;

        // Check for matching anchors and process prediction
        for (auto& anchor : anchors) {
            if (!anchor.isActive) continue; // Skip inactive anchors

            anyActiveAnchor = true; // Indicates at least one anchor is active

            if (anchor.pattern == window) {
                foundMatchingAnchor = true;
                // Calculate prediction probability
                float predictedProb = (anchor.nHits + alpha) / (anchor.nTries + 2 * alpha);
                // Assume the predicted symbol is the last byte of the anchor's pattern for simplicity
                char predictedSymbol = anchor.pattern.back();

                if (byte == predictedSymbol) {
                    anchor.recordHit();
                    float bits = -log2(predictedProb); // Calculating the bits for this successful prediction
                    nbits += bits;
                    encodedChars++;
                } else {
                    anchor.recordMiss();
                    float compProb = (1 - predictedProb) / 3;
                    float bits = -log2(compProb); // Adjusted for the incorrect prediction
                    nbits += bits;
                    encodedChars++;
                }

                // Check if the anchor's hit ratio falls below the threshold after this update
                if (anchor.getHitRatio() < threshold) {
                    anchor.deactivate();
                    anyActiveAnchor = false; // Might need to re-check the overall active status of anchors after this loop
                }
            }
        }

        // If no matching anchor was found for the current window, consider adding or reactivating anchors
        if (!foundMatchingAnchor) {
            // Reactivate the best anchor if all are inactive
            nonEncodedChars++;
            if (!anyActiveAnchor && !anchors.empty()) {
                auto bestIt = max_element(anchors.begin(), anchors.end(),
                                          [](const Anchor& a, const Anchor& b) { return a.getHitRatio() < b.getHitRatio(); });
                if (bestIt != anchors.end()) {
                    bestIt->isActive = true;  // Reactivate the best performing anchor
                    anyActiveAnchor = true;  // We now have an active anchor again
                }
            }

            // Try to add a new anchor if the current window pattern exists in the hash table
            if (hashTable.find(window) != hashTable.end() && anchors.size() < MAX_ANCHORS) {
                bool isAlreadyAnAnchor = find_if(anchors.begin(), anchors.end(), [&](const Anchor& a) { return a.pattern == window; }) != anchors.end();
                if (!isAlreadyAnAnchor) {
                    anchors.emplace_back(window, position);  // Add new anchor
                }
            }
        }

        // Always update hash table with the new k-mer position
        hashTable[window] = position;

        position++;
    }


    file.close();


    cout << "\nTotal bits: " << nbits
         << "\nDefault bits (if encoded with fixed length): " << defaultNbits
         << "\nEncoded characters: " << encodedChars
         << "\nNon-encoded characters: " << nonEncodedChars << endl;

    return 0;
}
