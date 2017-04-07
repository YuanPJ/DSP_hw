#include <iostream>
#include <fstream>
#include <hmm.h>


using namespace std;

int main(int argc, char** argv) {
    // reading modellist.txt
    string modellist[5];
    fstream f;
    f.open(argv[1])
    for (int i = 0; i < 5; i++) 
    f >> modellist[i];

    // input models
    HMM models[5];
    for (int i = 0; i < 5; i++) 
        loadHMM(&models[i], modellist[i]);
   
    // read testing sequence
    fstream seq;
    seq.open(argv[2]);
    for (int i = 0; i < 2500; i++) {
        // viterbi algorithm parameters
        double delta[5][50][6]; // model = 5, T = 50, N = 6
        
        //preprocess sequence
        string str;
        seq >> str;
        int obs[50];
        for (int t = 0; t < 50; t++) 
            obs[t] = int(str[t]) - 65;

        
    }

   return 0;
}