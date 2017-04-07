#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include "hmm.h"
#include <math.h>

using namespace std;

int main(int argc, char **argv)
{
	// training option
	int iters = atoi(argv[1]);

	HMM model;
	loadHMM(&model, argv[2]);
	int state = model.state_num;

	// training stage
	for (int e = 0; e < iters; e++) {
		// several sum to update a, b, pi
		double sum_pi[6];
		double sum_a_up[6][6];
		double sum_a_down[6];
		double sum_b_up[6][6];
		double sum_b_down[6];
		for (int i = 0; i < 6; i++) {
			sum_pi[i] = sum_a_down[i] = sum_b_down[i] = 0.0;
			for (int j = 0; j < 6; j++)
				sum_a_up[i][j] = sum_b_up[i][j] = 0.0;
		}

		// reading seq_model
		fstream seq;
		seq.open(argv[3]);

		for (int g = 0; g < 10000; g++) {
			//  training parameters
			double alpha[50][6]; // sample, T = 50, N = 6
			double beta [50][6];
			double gamma[50][6];
			double epson[50][6][6]; // T = 50, from i = 6, to j = 6
			int    obs  [50];

			// preprocess sequence
			string str;
			seq >> str;
			for (int t = 0; t < 50; t++) 
				obs[t] = int(str[t]) - 65;

			// calculate alpha
			for (int i = 0; i < state; i++)
				alpha[0][i] = model.initial[i] * model.observation[obs[0]][i];
			for (int t = 1; t < 50; t++) {
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
			for (int t = 0; t < 50; t++) {
				double sum = 0;
				for (int i = 0; i < state; i++) {
					gamma[t][i] = alpha[t][i] * beta[t][i];
					sum += gamma[t][i];
				}
				for (int i = 0; i < state; i++) 
					gamma[t][i] /= sum;
			}

			// calculate epson
			for (int t = 0; t < 49; t++) {
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

			// update several sum
			for (int i = 0; i < 6; i++) {
				sum_pi[i] += gamma[0][i];
				for (int t = 0; t < 49; t++) {
					sum_a_down[i] += gamma[t][i];
					sum_b_down[i] += gamma[t][i];
					for (int j = 0; j < 6; j++)
						sum_a_up[i][j] += epson[t][i][j];
				}
				sum_b_down[i] += gamma[49][i];
				for (int t = 0; t < 50; t++) {
					sum_b_up[obs[t]][i] += gamma[t][i];
				}
			}
		}

		// update a, b, pi
		for (int i = 0; i < 6; i++) {
			model.initial[i] = sum_pi[i] * 0.0001;
			for (int j = 0; j < 6; j++) {
				model.transition[i][j]  = sum_a_up[i][j] / sum_a_down[i];
				model.observation[i][j] = sum_b_up[i][j] / sum_b_down[j];
			}
		}
		cerr << "\riteratons : " << setw(4) << e << " / " << iters;
	}
	FILE *fs;
	fs = fopen(argv[4], "w");
	dumpHMM(fs, &model);
	return 0;
}
