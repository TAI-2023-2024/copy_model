#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
//
using namespace std;

string filename; //name of file to compress
float threshold; //value to stop a copy model
float kmerSize; //size of sequence to use as anchors
float alpha; //smoothing factor

float nBits; //number of bits for encoding

unordered_map<char, int> alphabet;
unordered_map<string, int> hashTable;

unordered_map<string, string> getFlags(int argc, char* argv[]) {
    unordered_map<string, string> flags;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            string flag = arg.substr(2);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                flags[flag] = argv[++i];
            }
            else {
                flags[flag] = "";
            }
        }
        else if (arg.substr(0, 1) == "-") {
            string flag = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                flags[flag] = argv[++i];
            }
            else {
                flags[flag] = "";
            }
        }
    }
    return flags;
}

int processFlags(unordered_map<string, string> flags) {

    if (flags.count("f") ) {
        filename = flags.at("f");
    }
    else {
        cout << "No filename given\n";
        return 1;
    }

    if (flags.count("t")) {
        threshold = stof(flags.at("t"));
    }
    else {
        threshold = 0.8;
    }

    if (flags.count("a")) {
        alpha = stof(flags.at("a"));
    }
    else {
        alpha = 1;
    }

    if (flags.count("k")) {
        kmerSize = stof(flags.at("k"));
    }
    else {
        kmerSize = 5;
    }

    return 0;
}

int getAlphabet() {

    char byte;

    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening the file.\n";
        return 1;
    }

    while (file.get(byte)) {
        alphabet[byte]++;
    }

    // Close the file
    file.close();

    //// Print the counts of all characters
    //cout << "Counts of all characters in the string:\n";
    //for (const auto& pair : alphabet) {
    //    cout << "'" << pair.first << "': " << pair.second << endl;
    //}

    return 0;
}

int main(int argc, char* argv[]) {

    unordered_map<string, string> flags = getFlags(argc, argv);

    if (processFlags(flags) != 0) {
        cout << "Something went wrong\n";
        return 1;
    }

    getAlphabet();

    char byte;

    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening the file.\n";
        return 1;
    }

    string window = "";
    string completeString = "";
    int position = 0;

    while (file.get(byte)) {

        if (window.length() == kmerSize) {
            window.erase(0, 1);
        }

        window.push_back(byte);
        completeString.push_back(byte);
        
        if (window.length() == kmerSize) {
            hashTable[window] = position;
        }

        position++;
    }

    return 0;
}