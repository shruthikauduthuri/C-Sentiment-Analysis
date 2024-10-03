// CS201PGM8Sentiment 
// NAME:  

#include "Sentiment.h"
using namespace std;

int main()
{
    vector<wordList> sentList, posList, negList;

    // load vectors 
    loadSentiment(sentList, posList, negList);

    ofstream outputFile("output.txt");
    if (!outputFile.is_open()) {
        cerr << "Error opening output file" << endl;
        return 1;
    }

    // open output file 
    // open review files

    string fileName;
    for (unsigned int i = 1; i < 9; i++) {
        // open input file adding to_string(i) + ".txt" to review
        // if not able to open, print a message and continue
        // else process the file & close it
        fileName = "review" + to_string(i) + ".txt";
        ifstream reviewFile(fileName);

        if (!reviewFile.is_open()) {
            cerr << "Error opening " << fileName << endl;
            continue;
        }

        // Process the review
        outputFile << "PROCESSING FILE: " << fileName << "\n\n";
        // processFile(reviewFile, outputFile, sentList);
        processFile(reviewFile, outputFile, fileName, sentList, posList, negList);
        reviewFile.close();

    }



    //close the output file
}
 
