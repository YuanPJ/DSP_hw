#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "hmm.h"
#include <math.h>

using namespace std;

int main()
{
/*
	HMM hmms[5];
	load_models( "modellist.txt", hmms, 5);
	dump_models( hmms, 5);
*/
	// training option
	int epochs = 1;

	//  training parameters
	double alpha[50][6]; // T = 50, N = 6
	double beta [50][6];
	double gamma[50][6];
	double epson[50][6][6]; // T = 50, from i = 6, to j = 6
	HMM model;
	loadHMM( &model, "model_init.txt" );
	int state = model.state_num;

	// reading seq_model
	fstream seq;
	seq.open("../seq_model_01.txt");

	// training stage
	for (int e = 0; e < epochs; e++) {
		for (int g = 0; g < 10000; g++) {
			// preprocess sequence
			string str;
			seq >> str;
			vector<int> obs;
			obs.resize(50);
			for (int j = 0; j < obs.size(); j++) 
				obs[j] = int(str[j]) - 65;

			// calculate alpha
			for (int i = 0; i < state; i++)
				alpha[0][i] = model.initial[i] * model.observation[obs[0]][i];
			for (int t = 1; t < obs.size(); t++) {
				for (int j = 0; j < state; j++) {
					double sum = 0;
					for (int i = 0; i < state; i++) 
						sum += alpha[t-1][i] * model.transition[i][j];
					alpha[t][j] = sum * model.observation[obs[t]][j];
				}
			}

			// calculate beta
			for (int i = 0; i < state; i++)
				beta[49][i] = 1;
			for (int t = 48; t >= 0; t--) {
				for (int i = 0; i < state; i++) {
					beta[t][i] = 0;
					for (int j = 0; j < state; j++) 
						beta[t][i] += model.transition[i][j] * 
									  model.observation[obs[t+1]][j] * beta[t+1][j];
				}
			}

			// calculate gamma
			for (int t = 0; t < obs.size(); t++) {
				double sum = 0;
				for (int i = 0; i < state; i++) {
					gamma[t][i] = alpha[t][i] * beta[t][i];
					sum += gamma[t][i];
				}
				for (int i = 0; i < state; i++) 
					gamma[t][i] /= sum;
			}

			// calculate epson
			for (int t = 0; t < obs.size()-1; t++) {
				double sum = 0;
				for (int i = 0; i < state; i++) {
					for (int j = 0; j < state; j++) {
						epson[t][i][j] = alpha[t][i] * model.transition[i][j] * 
										 model.observation[obs[t+1]][j] * beta[t+1][j];
						sum += epson[t][i][j];		
					}
				}
				for (int i = 0; i < state; i++) 
					for (int j = 0; j < state; j++)
						epson[t][i][j] /= sum;
			}

			// update pi (initial)
			for (int i = 0; i < state; i++)
				model.initial[i] = gamma[0][i];
			// update a  (transition)
			for (int i = 0; i < state; i++) {
				for (int j = 0; j < state; j++) {

				}
			}
			/*
			for (int i = 5; i >= 0; i--) {
				for (int t = 0; t < 10; t++)
					cout << gamma[t][i] << ' ';
				cout << endl;
			}
			cout << endl;
			*/
		}

		
	}


	return 0;
}
