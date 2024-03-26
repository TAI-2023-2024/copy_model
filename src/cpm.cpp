#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <math.h>

using namespace std;

string filename;
double threshold;
double kmerSize
double alpha;

double nbits = 0;
double defaultNbits = 0;

int encodedChars = 0;
int nonEncodedChars = 0;

int totalChars = 0;
int CharsLeft = 0;
int alphabetSize = 0;

unordered_map<char, int> alphabet;
unordered_map<string, int> hashTable;

string completeString = "";
char mostFrequent = ' ';
string lastCompressionResults = "";

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
        kmerSize = 10;
    }

    return 0;
}

int getAlphabetFrequency() {

    char byte;

    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening the file.\n";
        return 1;
    }

    while (file.get(byte)) {
        completeString += byte;
        alphabet[byte]++;
        totalChars++;
    }

    file.close();

    alphabetSize = alphabet.size();

    return 0;
}

int getMostFrequentChar()
{
    mostFrequent = alphabet.begin()->first;

    for (pair<char, int> c : alphabet)
    {
        mostFrequent = c.second <= alphabet[mostFrequent] ? mostFrequent : c.first;
    }

    return 0;
}

double predictProbability(int hits, int misses, double alpha) {
    return (hits + alpha) / (hits + misses + 2 * alpha);
}

double InfoBits(char ActualByte, char predictedSymbol, double predictedSymbolProb)
{
    if (ActualByte == predictedSymbol)
    {
        return ceil(-log2(predictedSymbolProb));
    }
    else
    {
        return ceil(-log2((1 - predictedSymbolProb) / (alphabetSize - 1)));
    }
}


int main(int argc, char* argv[]) {

    auto start = chrono::high_resolution_clock::now();

    unordered_map<string, string> flags = getFlags(argc, argv);

    if (processFlags(flags) != 0) {
        cout << "Something went wrong\n";
        return 1;
    }

    getAlphabetFrequency();

    auto alphabetInfo = alphabetSize * 5 * 8 + 8*2; //each character in the alphabet is a byte and each int is 4 bytes, plus 2 bytes for start and end of metadata

    nbits += alphabetInfo;
    
    CharsLeft = totalChars;

    char byte;

    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening the file.\n";
        return 1;
    }

    string window = "";

    double defaultNbitsPerChar = ceil(log2(alphabetSize));

    int position = 0;
    int misses;
    int hits;
    bool activeModel = false;
    int testingPosition;
    int offset;
    string currentAnchor;

    double correctSymbolProbability;
    double wrongSymbolProbability;
    double info;
    double prob;
    char predictedSymbol;

    int tempHits;
    int tempMisses;

    while (position < totalChars) {
        byte = completeString[position];

        defaultNbits += defaultNbitsPerChar;

        if (activeModel) {
            correctSymbolProbability = predictProbability(hits, misses, alpha);
            predictedSymbol = completeString[testingPosition + offset];
            if (byte == predictedSymbol) {
                hits++;
                info = -log2(correctSymbolProbability);
                lastCompressionResults += "1";
            }
            else {
                wrongSymbolProbability = (1 - correctSymbolProbability) / (alphabetSize - 1);
                misses++;
                info = -log2(wrongSymbolProbability);
                lastCompressionResults += "0";
            }
            nbits += info;
            encodedChars++;
            offset++;

            alphabet[byte]--;
            CharsLeft--;

            if (lastCompressionResults.length() == kmerSize+1) {
                lastCompressionResults.erase(0, 1);
                
                tempHits = count(lastCompressionResults.begin(), lastCompressionResults.end(), '1');
                tempMisses = kmerSize - tempHits;
                prob = predictProbability(tempHits, tempMisses, alpha);

                if (prob < threshold) {
                    activeModel = false;
                }
            }
        } else {
            getMostFrequentChar();
            double p = (double)alphabet[mostFrequent] / (double)CharsLeft;
            nbits += InfoBits(byte, mostFrequent, p);

            nonEncodedChars++;

            alphabet[byte]--;
            CharsLeft--;
        }

        if (window.length() == kmerSize) {
            window.erase(0, 1);
        }

        window += byte;

        if (window.length() == kmerSize) {
            if (completeString.length() > kmerSize && (hashTable.find(window) != hashTable.end()) && !activeModel) {
                hits = 0;
                misses = 0;
                activeModel = true;
                testingPosition = hashTable[window];
                offset = 0;
                currentAnchor = window;
            }

            hashTable[window] = position+1;
        }

        position++;
    }

    file.close();;

    auto stop = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::seconds>(stop - start);

    //cout << kmerSize << "," << alpha << "," << threshold << "," << nbits << "," << defaultNbits << "," << encodedChars << "," << nonEncodedChars << "," << nbits / totalChars << "," << defaultNbits / totalChars << "," << duration.count() << "\n";
    cout << "KmerSize: " << kmerSize << "\n";
    cout << "Alpha: " << alpha << "\n";
    cout << "Threshold: " << threshold << "\n";
    cout << "Nbits: " << nbits << "\n";
    cout << "Default Nbits: " << defaultNbits << "\n";
    cout << "Encoded chars: " << encodedChars << "\n";
    cout << "Non Encoded chars: " << nonEncodedChars << "\n";
    cout << "Bits per char: " << nbits / totalChars << "\n";
    cout << "Default Bits per char: " << defaultNbits / totalChars << "\n";
    cout << "Duration (seconds) : " << duration.count() << "\n";

    return 0;
}