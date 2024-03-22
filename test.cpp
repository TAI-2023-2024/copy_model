#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <sstream>
//
using namespace std;

string filename; //name of file to compress
float threshold; //value to stop a copy model
float kmerSize; //size of sequence to use as anchors
float alpha; //smoothing factor

unsigned int nbits = 0; //number of bits for encoding
unsigned int defaultNbits = 0;

int encodedChars = 0;
int nonEncodedChars = 0;

unsigned int totalChars = 0;
unsigned int alphabetSize = 0;
unsigned int CharsLeft = 0;
char AlphaChar = ' '; // alphabet char with greatest frequency

unordered_map<char, int> alphabet;
unordered_map<string, int> hashTable;
unordered_map<string, int> kmerCounter;
unordered_map<int, int> averageSizeOfCopy;

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
        //cout << "No filename given\n";
        //return 1;
        //filename = "C:\\Users\\admin\\Desktop\\UA\\TAI\\copy_model\\chry.txt";
        filename = ".\\chry.txt";
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
        kmerSize = 8;
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
        alphabet[byte]++;
        totalChars++;
    }

    // Close the file
    file.close();

    alphabetSize = alphabet.size();

    //// Print the counts of all characters
    //cout << "Counts of all characters in the string:\n";
    //for (const auto& pair : alphabet) {
    //    cout << "'" << pair.first << "': " << pair.second << endl;
    //}

    return 0;
}

int getMAXAlphabetFrequency()
{
    if(alphabet.size() == 0)
    {
        cout << "The files alphabet is null"<< endl;
        return 1;
    }

    AlphaChar = alphabet.find(AlphaChar) != alphabet.end()? AlphaChar : alphabet.begin()->first;

    for(pair<char,int> c : alphabet)
    {
         AlphaChar = c.second<= alphabet[AlphaChar]? AlphaChar : c.first;
    }

    return 0;
}

float predictProbability(int hits, int misses, float alpha) {
    return (hits + alpha) / (hits + misses + 2 * alpha);
}

unsigned int InfoBits(char ActualByte, char predictedSymbol, float predictedSymbolProb)
{    
    if (ActualByte == predictedSymbol)
    {
        //hits++;
        return ceil(-log2(predictedSymbolProb));
    }
    else 
    {
        //wrongSymbolProbability = (1 - correctSymbolProbability) / (alphabetSize - 1);
        //misses++;
        return ceil(-log2((1 - predictedSymbolProb) / (alphabetSize - 1)));
    }           
}

int main(int argc, char* argv[]) {

    unordered_map<string, string> flags = getFlags(argc, argv);

    if (processFlags(flags) != 0) {
        cout << "Something went wrong\n";
        return 1;
    }

    getAlphabetFrequency();
    CharsLeft = totalChars;

    char byte;

    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening the file.\n";
        return 1;
    }

    string window = "";
    string completeString = "";

    float defaultNbitsPerChar = ceil(log2(alphabetSize));

    int position = 0;
    int misses;
    int hits;
    bool activeModel = false;
    int testingPosition;
    int offset;
    string currentAnchor;

    float correctSymbolProbability;
    float wrongSymbolProbability;
    float info;
    float prob;
    char predictedSymbol;

    while (file.get(byte)) {
        //defaultNbits += defaultNbitsPerChar;
        defaultNbits = defaultNbits + 8;

        //if (position % 1000 == 0) {
        //    cout << position << " out of " << totalChars - 1 << ", " << ((float)position) / ((float)totalChars - 1) * 100 << "%" << '\r';
        //}

        if (activeModel) {
            correctSymbolProbability = predictProbability(hits, misses, alpha);
            predictedSymbol = completeString[testingPosition + offset];
            if (byte == predictedSymbol) {
                hits++;
                info = ceil(-log2(correctSymbolProbability));
            }
            else {
                wrongSymbolProbability = (1 - correctSymbolProbability) / (alphabetSize - 1);
                misses++;
                info = (-log2(wrongSymbolProbability));
            }
            nbits += info;
            encodedChars++;
            offset++;
            
            alphabet[byte]--; 
            CharsLeft--;

            prob = predictProbability(hits, misses, alpha);
            if (prob < threshold) {
                activeModel = false;
                if (averageSizeOfCopy.find(offset) == averageSizeOfCopy.end()) {
                    averageSizeOfCopy[offset] = 0;
                }
                else {
                    averageSizeOfCopy[offset]++;
                }
            }
        } else {
            //nbits += defaultNbitsPerChar;
            
            //Predict symbol in acordance to the alphabet
            getMAXAlphabetFrequency();
            float p = (float)alphabet[AlphaChar] / (float)CharsLeft;
            nbits += InfoBits(byte,AlphaChar,p);              

            nonEncodedChars++;

            alphabet[byte]--; 
            CharsLeft--;
        }

        if (window.length() == kmerSize) {
            window.erase(0, 1);
        }

        window.push_back(byte);
        completeString.push_back(byte);

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
            if (kmerCounter.find(window) == kmerCounter.end()) {
                kmerCounter[window] = 0;
            }
            else {
                kmerCounter[window]++;
            }
        }


        position++;
    }

    file.close();

    cout << "\n";
    cout << "Nbits: " << nbits << "\n";
    cout << "Default Nbits: " << defaultNbits << "\n";
    cout << "Encoded chars: " << encodedChars << "\n";
    cout << "Non Encoded chars: " << nonEncodedChars << "\n";

   /* std::for_each(
        kmerCounter.begin(), kmerCounter.end(),
        [](pair<string, int> p) {

            cout << p.first << " :: " << p.second
                << endl;
        });*/


    return 0;
}