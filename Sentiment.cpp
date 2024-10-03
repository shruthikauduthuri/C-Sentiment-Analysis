#include "Sentiment.h"
#include <fstream>
#include <sstream>
#include <algorithm>  
#include <cctype>     
#include <iostream>
#include <iomanip>
#include <vector>

// Function to compare two wordList objects for sorting
bool compareWordList(const wordList &a, const wordList &b) {
    return a.word < b.word;
}

// Function to perform binary search to find sentiment value
double binarySearchSentiment(const string& word, const vector<wordList>& sentList) {
    int low = 0, high = sentList.size() - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (sentList[mid].word == word) {
            return sentList[mid].value; // Return the sentiment value if found
        } else if (sentList[mid].word < word) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return 0.0; // Return 0 if word not found
}


// Function to remove punctuation from a string
string removePunctuation(const string& str) {
    string result;
    for (char ch : str) {
        if (!ispunct(ch)) {
            result += ch;
        }
    }
    return result;
}


//SUPPORT FUNCTION TO FIND THE CURRENT WORD IN THE SENTEMENT VECTOR & RETURN THE SENTIMENT VALUE
//PRE:   accepts a string (in) and the words list
//POST:  returns the sentiment of the word if found or the value 0 if not

double sentimentVal(string in, vector<wordList>& sentList) {

    // Convert the word to lowercase
    transform(in.begin(), in.end(), in.begin(), ::tolower);

    // Remove punctuation
    in = removePunctuation(in);

    // Use binary search to find the sentiment value of the word
        return binarySearchSentiment(in, sentList);
    return 0;

} 


//PRE:   accepts an empty set of 3 <wordList> vectors
//POST:  loads all three vectors with data from sentiment.txt
void loadSentiment(vector<wordList>& sentList,
    vector<wordList>& posList,
    vector<wordList>& negList) {

    //open sentiment.txt
    //add the word to the sentList vector
    //if positive enough add to posList
    //if negative enough add to negList
    //close sentiment.txt

    ifstream file("sentiment.txt");
    string line;

    if (!file.is_open()) {
        cout << "Error opening sentiment.txt" << endl;
        return;
    }

    while (getline(file, line)) {
        size_t commaPos = line.find(',');
        string word = line.substr(0, commaPos);
        double value = stod(line.substr(commaPos + 1));

        wordList temp(word, value);
        sentList.push_back(temp);

        // Add to positive or negative list if conditions are met
        if (value > 1.25) {
            posList.push_back(temp);
        } else if (value < -1.25) {
            negList.push_back(temp);
        }
    }

    file.close();

    // Sort the sentList by words for binary search
      sort(sentList.begin(), sentList.end(), compareWordList);
 
}



//PRE:  positive & negative word lists are sent
//POST: Open the input file & output file (infile+Output 
//      'Clean' the input by removing syntax and setting all to lower case
//       Get the sentiment value for the edited word
//        store unedited word in vector with sentiment value
//         if sentiment < -1  find a positive word to replace it
//         if sentiment > 1   find a negative word to replace

void processFile(ifstream& inFile, ofstream& outFile, string fileName,
                 vector<wordList>& words, vector<wordList>& posList, vector<wordList>& negList) {
    string inWord, newWord;
    char newChar;
    double sentiment = 0; 
    vector<wordList> origWords, negWords, posWords;
    bool updatedToNegative = false, updatedToPositive = false;

    while (inFile >> inWord) {
        // Clean the word
        newWord = "";
        for (char ch : inWord) {
            if (isalpha(ch)) {
                newWord += tolower(ch);
            }
        }
        inWord.erase(remove_if(inWord.begin(), inWord.end(), ::ispunct), inWord.end());

        // Find the sentiment value for the cleaned word
        double wordSentiment = sentimentVal(newWord, words);
        sentiment += wordSentiment;

        // Store the word and its sentiment value
        origWords.push_back(wordList(inWord, wordSentiment));
    }

    // Output the original formatted review (limit lines to 80 characters)
    outFile << "FORMATTED REVIEW: " << endl;

    int lineLength = 0;
    for (const auto& w : origWords) {
        if (lineLength + w.word.length() > 80) {
            outFile << endl;
            lineLength = 0;
        }
        outFile << w.word << " ";
        lineLength += w.word.length() + 1;
    }

    outFile << endl << endl; 
    outFile << "ORIGINAL SENTIMENT: " << sentiment << endl;
    outFile << endl << endl;

    // Now, modify the review to make it more negative and positive

    outFile << "WORDS UPDATED TO BE MORE NEGATIVE:" << endl;
    double totalPosSentiment = 0, totalNegSentiment = 0;
    for (auto& w : origWords) {
        if (w.value > 1) {
            // Replace with a random negative word
            int idx = rand() % negList.size();
            outFile << setw(20) << w.word 
                << setw(20) << w.value 
                << setw(20) << negList[idx].word 
                << setw(20) << negList[idx].value 
                << endl;
            totalPosSentiment += w.value;
            totalNegSentiment += negList[idx].value;
            w.word = negList[idx].word;
            updatedToNegative = true;
        }
    }


    // Output the modified review with more negative words
    double updatedSentiment = 0;
    if (updatedToNegative) {
        // Output the totals
        outFile << "TOTALS: " << setw(32) << totalPosSentiment
                << setw(41) << totalNegSentiment << endl;

        outFile << endl << endl;
        // Output the modified review with more negative words
        outFile << "FORMATTED REVIEW (More Negative): " << endl;
        lineLength = 0;
        for (const auto& w : origWords) {
            if (lineLength + w.word.length() > 80) {
                outFile << endl;
                lineLength = 0;
            }
            outFile << w.word << " ";
            lineLength += w.word.length() + 1;
        }
        outFile << endl << endl;
        // Calculate and display the updated sentiment
        for (const auto& w : origWords) {
            updatedSentiment += sentimentVal(w.word, words);
        }
        outFile << "UPDATED SENTIMENT: " << fixed << setprecision(2) << updatedSentiment << endl;
        outFile << endl << endl;
    } else {
        outFile << "REVIEW NOT UPDATED TO BE MORE NEGATIVE. THE SENTIMENT REMAINS: " 
                << fixed << setprecision(2) << sentiment << endl;
    }

    // Similar process to make the review more positive

    outFile << "WORDS UPDATED TO BE MORE POSITIVE:" << endl;
    outFile << endl << endl;
    totalPosSentiment = 0, totalNegSentiment = 0;
    for (auto& w : origWords) {
        if (w.value < -1) {
            // Replace with a random positive word
            int idx = rand() % posList.size();
            outFile << setw(20) << w.word 
                << setw(20) << w.value 
                << setw(20) << posList[idx].word 
                << setw(20) << posList[idx].value
                << endl;
            totalNegSentiment += w.value;
            totalPosSentiment += posList[idx].value;
            w.word = posList[idx].word;
        }
    }

    updatedSentiment = 0;
    // Output the modified review with more positive words
    if (updatedToPositive) {
        // Output the totals
        outFile << "TOTALS: " << setw(32) << totalNegSentiment
                << setw(41) << totalPosSentiment << endl;

        outFile << endl << endl;
        // Output the modified review with more positive words
        outFile << "FORMATTED REVIEW (More Positive): " << endl;
        lineLength = 0;
        for (const auto& w : origWords) {
            if (lineLength + w.word.length() > 80) {
                outFile << endl;
                lineLength = 0;
            }
            outFile << w.word << " ";
            lineLength += w.word.length() + 1;
        }
        outFile << endl << endl;

        // Calculate and display the updated sentiment
        for (const auto& w : origWords) {
            updatedSentiment += sentimentVal(w.word, words);
        }
        outFile << "UPDATED SENTIMENT: " << fixed << setprecision(2) << updatedSentiment << endl;

    } else {
        outFile << "REVIEW NOT UPDATED TO BE MORE POSITIVE. THE SENTIMENT REMAINS: " << sentiment << endl;
        outFile << endl << endl;
    }
}