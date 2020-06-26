#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <time.h>
#include <thread>
#include <omp.h>
#include <unordered_map>


using namespace std;

const int num_threads = 8;


vector<string> myReadFile(std::string fileName){
    ///function used to read the words from a text file

    ifstream inputFile(fileName);
    regex letter("[^[:alpha:]]"); //regex used for put in the vector only words with alphabetic characters

    vector<string> book;

    if (inputFile.is_open()) {
        string line;

        while (getline(inputFile, line))//leggo le linee
        {
            string word;
            line = regex_replace(line, letter, " ");//replace with spaces characters not allowed

            stringstream line2(line);
            while (getline(line2, word, ' ')) {//cut one text line in words
                if (word != "") {
                    for (int i = 0; i < word.size(); i++) {
                        word[i] = tolower(word[i]);//turns the word into lowercase
                    }

                    book.push_back(word);

                }

            }

        }
        inputFile.close();

    }

    return book;
}

void myWriteFile(string fileName, vector<string> nGrams){
    ///function used to write ngrams in the vector into a text file

    ofstream oFile(fileName, ios_base::out | ios_base::trunc);

    if (oFile.is_open())
    {
        for (vector<string>::iterator it = nGrams.begin(); it != nGrams.end(); ++it)//scroll with iterator and write
            oFile << *it + " ";

    }
    oFile.close();
}

void generateNgrams(int n, vector<string> strList,vector<string> *ngrams) {
    ///function that generate n-grams from an list of words sequentially
    ///for each word of dimension 'm' it sets the limit for the second for cicle and generate 'm - n + 1' n-grams.
    int limit;
    string word;
    for (int j = 0; j<strList.size(); j++) {
        word = strList[j];
        limit = word.length() - n + 1;
            for (int i = 0; i < limit; i++) {
                ngrams->push_back(word.substr(i, n));
            }
    }

}

void compute(int thread_index, string word, int n, vector<string> *ngrams){
    ///auxiliary function that calculates n-grams of a word. Used for parallel OpenMP version.
    int limit;
    //vector<string> ngrams;
    if(!word.empty())
        limit = word.length() - n + 1;
        for(int i = 0; i < limit; i++){
            //total_ngrams[thread_index].push_back(word.substr(i,n));
            ngrams[thread_index].push_back(word.substr(i,n));
        }

}


void generateNgramsParallel(int n, vector<string> strList, vector<string>* ngrams) {
    ///function that generate n-grams of a word list using OpenMP.
    ///this works partitioning the word list into a numer of blocks equal to thread number

    string word;
    int thread_index;

    #pragma omp parallel private(thread_index,word) num_threads(num_threads)
    //private variables so that each thread works on its copy in memory
    {

        thread_index = omp_get_thread_num();

        #pragma omp for
        for (int j = 0; j <strList.size(); j++) {
            word = strList[j];
            compute(thread_index, word, n, ngrams);
        }
    }

}

void frequency(vector<string> ngrams){
    ///method that compute the frequency of each n-gram in the n-grams list

    unordered_map<string, double> freq;

    for( int i = 0; i < ngrams.size(); i++){
        freq[ngrams[i]]++;
    }
    unordered_map<string, double>:: iterator p;

    //normalization
    for (p = freq.begin(); p != freq.end(); p++)
        p->second = p->second / ngrams.size();

    //print each n-gram with the associated normalizated frequence
    cout << "FREQUENCY OF EACH N-GRAM -----------------" << endl;
    for (p = freq.begin(); p != freq.end(); p++)
        cout << "(" << p->first << ", " << p->second << ")\n";

}




int main()
{

    clock_t start,end;
    double time1,time2;

    vector<string> book = myReadFile("orgoglio_75kb.txt");
    int n = 3;

    vector<string> ngrams1,ngrams2_to_print;
    vector<string> ngrams2[num_threads];//vector of vectors, used to make each thread write on its own vector avoiding conflicts.

    for (int i = 0; i<3; i++) {
        start = clock();
        generateNgrams(n, book, &ngrams1);
        end = clock();
        time1 = ((double) (end - start)) / CLOCKS_PER_SEC;
        myWriteFile("ngrams1.txt", ngrams1);
        cout << "Sequential program executed in " << time1 * 1000 << " ms" << endl;
        ngrams1.clear();

        start = clock();
        generateNgramsParallel(n, book, ngrams2);
        end = clock();
        time2 = ((double) (end - start)) / CLOCKS_PER_SEC;
        //myWriteFile("ngrams2.txt", ngrams2);

        //Put all the n-grams generated from each thread in a single vector to make it compatible with the myWriteFile() function.
        for (auto &part:ngrams2) {
            for (auto &par: part)
                ngrams2_to_print.push_back(par);
            part.clear();
        }
        myWriteFile("ngrams2.txt", ngrams2_to_print);
        ngrams2_to_print.clear();
        cout << "Parallel(OpenMP) program executed in " << time2 * 1000 << " ms" << endl;
        //frequency(ngrams2_to_print);
    }
    return 0;
}