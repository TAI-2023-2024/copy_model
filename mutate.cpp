#include <iostream>
#include <string>
#include <set>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <random>

using namespace std;

string inputFilename;
string outputFilename;
float mutationProbability;
int alphabetSize;

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

    if (flags.count("f")) {
        inputFilename = flags.at("f");
    }
    else {
        cout << "No filename given\n";
        return 1;
    }

    if (flags.count("p")) {
        mutationProbability = stof(flags.at("p"));
    }
    else {
        mutationProbability = 0.1;
    }

    return 0;
}

vector<char> getAlphabet() {

    char byte;
    set<char> alphabetSet;

    ifstream file(inputFilename);

    if (!file.is_open()) {
        cerr << "Error opening the file.\n";
        vector<char> emptyVector;
        return emptyVector;
    }

    while (file.get(byte)) {
        alphabetSet.insert(byte);
    }

    // Close the file
    file.close();

    vector<char> alphabet(alphabetSet.begin(), alphabetSet.end());

    alphabetSize = alphabet.size();

    return alphabet;
}

string getFilename(const std::string& filePath) {
    int pos = filePath.find_last_of("\\/");
    if (pos != string::npos) {
        return filePath.substr(pos + 1);
    }
    else {
        return filePath;
    }
}

int main(int argc, char* argv[]) {

    unordered_map<string, string> flags = getFlags(argc, argv);

    if (processFlags(flags) != 0) {
        cout << "Something went wrong\n";
        return 1;
    }

    vector<char> alphabet = getAlphabet();

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> randomValue(0.0, 1.0);
    uniform_int_distribution<int> charToMutateInto(0, alphabetSize-1);

    char byte;
    char mutationByte;

    string filename = getFilename(inputFilename);

#ifdef _WIN32
    outputFilename = "Data\\mutated_" + to_string(mutationProbability) + "_" + filename;
#else
    outputFilename = "Data/mutated_" + to_string(mutationProbability) + "_" + filename;
#endif

    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);

    if (!inputFile.is_open()) {
        cerr << "Error opening the file.\n";
        return 1;
    }

    if (!outputFile.is_open()) {
        cerr << "Test opening the file.\n";
        return 1;
    }

    int mutationCounter = 0;
    int falseMutationCounter = 0;

    while (inputFile.get(byte)) {
        if (randomValue(gen) < mutationProbability) {
            mutationByte = alphabet[charToMutateInto(gen)];
            outputFile << mutationByte;
            mutationCounter++;
            if (mutationByte == byte) {
                falseMutationCounter++;
            }
        }
        else {
            outputFile << byte;
        }
    }

    inputFile.close();
    outputFile.close();


    cout << filename << "," << mutationProbability << "," << mutationCounter << "," << mutationCounter - falseMutationCounter << "," << falseMutationCounter << "\n";
    /*
    cout << "Filename: " << filename << "\n";
    cout << "Mutation Probability: " << mutationProbability << "\n";
    cout << "Mutations: " << mutationCounter << "\n";
    cout << "True Mutations: " << mutationCounter - falseMutationCounter << "\n";
    cout << "False Mutations: " << falseMutationCounter << "\n";*/

    return 0;
}