#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
//
//using namespace std;

std::string filename; //name of file to compress
float threshold; //value to stop a copy model
float kmerSize; //size of sequence to use as anchors
float alpha; //smoothing factor

float nBits; //number of bits for encoding

std::unordered_map<char, int> alphabet;
std::unordered_map<std::string, int> hashTable;

std::unordered_map<std::string, std::string> getFlags(int argc, char* argv[]) {
    std::unordered_map<std::string, std::string> flags;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            std::string flag = arg.substr(2);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                flags[flag] = argv[++i];
            }
            else {
                flags[flag] = "";
            }
        }
        else if (arg.substr(0, 1) == "-") {
            std::string flag = arg.substr(1);
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

int processFlags(std::unordered_map<std::string, std::string> flags) {

    if (flags.count("f") ) {
        filename = flags.at("f");
    }
    else {
        std::cout << "No filename given\n";
        return 1;
    }

    if (flags.count("t")) {
        threshold = std::stof(flags.at("t"));
    }
    else {
        threshold = 0.8;
    }

    if (flags.count("a")) {
        alpha = std::stof(flags.at("a"));
    }
    else {
        alpha = 1;
    }

    if (flags.count("k")) {
        kmerSize = std::stof(flags.at("k"));
    }
    else {
        kmerSize = 5;
    }

    return 0;
}


int main(int argc, char* argv[]) {

    std::unordered_map<std::string, std::string> flags = getFlags(argc, argv);

    if (processFlags(flags) != 0) {
        std::cout << "Something went wrong\n";
        return 1;
    }

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening the file.\n";
        return 1;
    }

    std::stringstream buffer;

    buffer << file.rdbuf(); // Read the file content into the stringstream

    // Close the file
    file.close();

    // Convert the stringstream to a string
    std::string fileContent = buffer.str();

    for (char ch : fileContent) {
        alphabet[ch]++;
    }

    // Print the counts of all characters
    std::cout << "Counts of all characters in the string:\n";
    for (const auto& pair : alphabet) {
        std::cout << "'" << pair.first << "': " << pair.second << std::endl;
    }

    return 0;
}