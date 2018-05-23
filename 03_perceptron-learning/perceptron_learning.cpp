// 10/07/2016 - CS 5600 - Meine, Joel
// Homework #3 - Perceptron Learning

#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
using namespace std;

typedef struct {
	double X1, X2, T;
} sample;
int samples_count = 0;

double perceptron(vector<double> X, vector<double> W)
{
	double h = 0;
	double x1 = X[0], x2 = X[1];
	double w0 = W[0], w1 = W[1], w2 = W[2];
	vector<double> x = { w0, x1, x2 };
	vector<double> w = { w0, w1, w2 };
	for (int i = 0; i < 3; i++)
	{
		h = h + (w[i] * x[i]);
	}
	return(h);
}

typedef struct {
	double X1, X2, G;
	bool Class;
} point;

point sigmoid(double h, vector<double> x)
{
	point g;
	g.X1 = x[0]; g.X2 = x[1];
	g.G = 1 / (1 + exp(-h));
	if (g.G > 0.5)
		g.Class = true;
	else
		g.Class = false;
	return(g);
}

vector<double> delta_weight(double alpha, double t, point g)
{
	double w;
	vector<double> d, x;
	x.push_back(g.X1); x.push_back(g.X2);
	for (int i = 0; i < 2; i++)
	{
		w = alpha*(t - g.G)*(g.G*(1 - g.G))*x[i];
		d.push_back(w);
	}
	return(d);
}

double error(vector<double> t, vector<point> P, int n)
{
	double e = 0;
	for (int j = 0; j < n; j++)
		e = e + pow(t[j] - P[j].G, 2);
	e = 0.01 * e;
	return(e);
}

typedef struct {
	double W0, W1, W2;
	vector<point> Points;
	vector<int> Epocs;
	vector<double> Errors;
	int Points_Size, Epocs_Size, Errors_Size;
} result;

result neural_network(vector<sample> samples, int samples_size)
{
	/* 1. Initialize the variables and arrays. */
	result NN;
	double w0 = 1, w1, w2;
	double x1 = 0, x2 = 0, t = 0, h = 0; point g;
	vector<double> X = {}, W = {}, T = {}, H = {};
	vector<point> G = {}; int G_count = 0;
	double err = 1;
	vector<double> errs = {}; int errs_count = 0;
	double err_limit = 0.0001;
	int loops = 0;
	int epoc = 1000;
	vector<int> epocs = {}; int epocs_count = 0;
	double learning_rate = 0.001;
	double dw1 = 0, dw2 = 0;
	vector<double> dw;

	/* 2. Generate a random weight value of either -0.2 or +0.2. */
	srand(time(0));
	w1 = (rand() % 40001 + (-20000)) * .00001;
	w2 = (rand() % 40001 + (-20000)) * .00001;

	/* 3. Train the neural network. */
	while (loops < epoc)
	{
		dw1 = 0, dw2 = 0;
		for (int i = 0; i < samples_size; i++)
		{
			x1 = samples[i].X1;
			x2 = samples[i].X2;
			X = { x1, x2 };
			t = samples[i].T;
			T.push_back(t);
			W = { w0, w1, w2 };
			h = perceptron(X, W);
			H.push_back(h);
			g = sigmoid(h, X);
			G.push_back(g);
			G_count++;
			dw = delta_weight(learning_rate, t, g);
			dw1 = dw[0]; dw2 = dw[1];
			w1 = w1 + dw1; w2 = w2 + dw2;
		}
		err = error(T, G, samples_size);
		errs.push_back(err);
		errs_count++;
		epocs.push_back(loops);
		epocs_count++;
		loops++;
		T = {}; H = {};
		if (loops != epoc)
		{
			G = {}; G_count = 0;
		}
		if (err < err_limit)
		{
			epocs = {};
			for (int i = 0; i <= epoc; i++)
			{
				epocs.push_back(i);
				epocs_count++;
			}
			for (int i = loops + 1; i <= epoc; i++)
			{
				errs.push_back(err_limit);
				errs_count++;
			}
			loops = epoc;
		}
	}

	/* 4. Save the neural network result. */
	NN.W0 = W[0]; NN.W1 = W[1]; NN.W2 = W[2];
	NN.Points = G; NN.Epocs = epocs; NN.Errors = errs;
	NN.Points_Size = G_count; NN.Epocs_Size = epocs_count; NN.Errors_Size = errs_count;
	return(NN);
}

/* The graph of a neural network showing epoc (x-axis) vs. average error (y-axis). */
void graph(result NN, string filename)
{
	ofstream PLAresults(filename);
	if (PLAresults.is_open())
	{
		for (int i = 0; i < NN.Epocs_Size; i++)
		{
			PLAresults << NN.Epocs[i] << " " << NN.Errors[i] << endl;
		}
		PLAresults.close();
	}
}

/* The graphs of multiple neural networks showing epoc (x-axis) vs. average error (y-axis). */
void graphs(vector<result> NNs, int NNs_size, string filename)
{
	ofstream PLAresults(filename);
	if (PLAresults.is_open())
	{
		for (int i = 0; i < NNs[0].Epocs_Size; i++)
		{
			PLAresults << NNs[0].Epocs[i];
			for (int j = 0; j < NNs_size; j++)
			{
				PLAresults << " " << NNs[j].Errors[i];
				if (j == NNs_size - 1)
					PLAresults << endl;
			}
		}
		PLAresults.close();
	}
}

/* The result of a point search in a neural network. */
bool search(point p, result NN)
/* The point is found if its nearest neighbor is the same class type. */
{
	point q;
	double dx, dy;
	vector<double> d; int d_size = 0;
	for (int i = 0; i < NN.Points_Size; i++)
	{
		/* Compare difference of input point to all points. */
		q = NN.Points[i];
		dx = p.X1 - q.X1;
		dy = p.X2 - q.X2;
		d.push_back(dx+dy);
		d_size++;
	}
	double min_d = 1;
	point best_match;
	for (int i = 0; i < d_size; i++)
	{
		/* Determine which point was closest to input point. */
		if (d[i] < min_d)
		{
			min_d = d[i];
			best_match = NN.Points[i];
		}
	}
	if (p.Class == best_match.Class)
		return(true);
	else
		return(false);
}

/* The cross validation of a list of samples. */
int training_set_size = 0, test_set_size = 0;
vector<vector<vector<double>>> cross_validation(vector<vector<double>> samples, int samples_count, int random_splits, double training_portion)
{
	vector<vector<vector<double>>> validation_set = {};
	vector<vector<double>> training_set = {};
	vector<vector<double>> test_set = {};
	int training = round(training_portion*samples_count);
	int test = round((1 - training_portion)*samples_count);
	for (int i = 0; i < random_splits; i++)
	{
		srand(time(0));
		random_shuffle(samples.begin(), samples.end());
		training_set = {};
		test_set = {};
		for (int ii = 0; ii < training; ii++)
		{
			training_set.push_back(samples[ii]);
			if (i == 0)
				training_set_size++;
		}
		for (int ii = training; ii < training + test; ii++)
		{
			test_set.push_back(samples[ii]);
			if (i == 0)
				test_set_size++;
		}
		validation_set.push_back(training_set);
		validation_set.push_back(test_set);
	}
	return(validation_set); /* Even=training_set Odd=test_set */
}

int main()
{
	/* 1. Initialize the test set of samples. */
	cout << "Initializing training set for PLA... ";
	ifstream dataset("Data.txt");
	char c; int i = 0, I = 0; bool start = false; bool negative = false; int var = 1;
	vector<int> buffer = { 1, 1, 1, 1, 1 };
	double x = 0;
	sample sam; vector<sample> samples;
	while (dataset.get(c))
	{
		if (start == true)
		{
			if (c == '0')
			{
				if (var != 3)
				{
					if (buffer[i] != -1)
						buffer[i] = 0;
					else
						negative = true;
				}
				else
					sam.T = 0;
			}
			else if (c == '1')
			{
				if (var != 3)
					buffer[i] = buffer[i] * 1;
				else
					sam.T = 1;
			}
			else if (c == '2')
				buffer[i] = buffer[i] * 2;
			else if (c == '3')
				buffer[i] = buffer[i] * 3;
			else if (c == '4')
				buffer[i] = buffer[i] * 4;
			else if (c == '5')
				buffer[i] = buffer[i] * 5;
			else if (c == '6')
				buffer[i] = buffer[i] * 6;
			else if (c == '7')
				buffer[i] = buffer[i] * 7;
			else if (c == '8')
				buffer[i] = buffer[i] * 8;
			else if (c == '9')
				buffer[i] = buffer[i] * 9;
			else if (c == '.')
				i--;
			else if (c == '-')
			{
				buffer[i] = -1; i--;
			}
			else if (c == '\t')
			{
				if (i == 4)
				{
					buffer[4] = 0;
				}
				else if (i == 3)
				{
					buffer[3] = 0;
					buffer[4] = 0;
				}
				else if (i == 2)
				{
					buffer[2] = 0;
					buffer[3] = 0;
					buffer[4] = 0;
				}
				for (int ii = 0; ii < 5; ii++)
				{
					if (ii == 0)
						x = x + 1.0*buffer[ii];
					else if (ii == 1)
					{
						if (x < 0)
							x = x - 0.1*buffer[ii];
						else
							x = x + 0.1*buffer[ii];
					}
					else if (ii == 2)
					{
						if (x < 0)
							x = x - 0.01*buffer[ii];
						else
							x = x + 0.01*buffer[ii];
					}
					else if (ii == 3)
					{
						if (x < 0)
							x = x - 0.001*buffer[ii];
						else
							x = x + 0.001*buffer[ii];
					}
					else if (ii == 4)
					{
						if (x < 0)
							x = x - 0.0001*buffer[ii];
						else
							x = x + 0.0001*buffer[ii];
					}
				}
				if (negative == true)
				{
					x = x + 1;
					negative = false;
				}
				if (var == 1)
				{
					sam.X1 = x; x = 0; var = 2;
					buffer = { 1, 1, 1, 1, 1 };
					i = -1;
				}
				else if (var == 2)
				{
					sam.X2 = x; x = 0; var = 3;
					buffer = { 1, 1, 1, 1, 1 };
					i = -1;
				}
			}
			else if (c == '\n')
			{
				samples.push_back(sam);
				samples_count++;
				var = 1;
				I = 1;
				if (dataset.eof())
					goto done;
			}
			if (I == 1)
			{
				i = 0; I = 0;
			}
			else
				i++;
		}
		else
		{
			if (c == '\n')
				start = true;
		}
	}
done: dataset.close();
	cout << "DONE" << endl; cout << "" << endl;

	/* 2. Create the neural network. */
	cout << "Creating neural network for PLA... ";
	result PLAresult = neural_network(samples, samples_count);
	cout << "DONE" << endl; cout << "" << endl;
	
	/* 3. Create graph data of epoc (x-axis) vs. average error (y-axis). */
	graph(PLAresult, "PLAresults.txt");
	cout << "PLA graph data has been saved to file." << endl; cout << "" << endl;

	/* 4. Test neural network with cross validation. */
	vector<double> pla_sample;
	vector<vector<double>> pla_samples = {};
	for (int i = 0; i < samples_count; i++)
	{
		pla_sample = { samples[i].X1, samples[i].X2 , samples[i].T };
		pla_samples.push_back(pla_sample);
	}
	cout << "Creating multiple training/test sets of randomly shuffled samples for PLA... ";
	int random_splits = 10; double training_portion = 0.8; int nn_count = 1;
	vector<vector<vector<double>>> plaCV = cross_validation(pla_samples, samples_count, random_splits, training_portion);
	cout << "DONE" << endl; cout << "" << endl;
	sample training_sam; vector<sample> training_sams; int training_sams_size = 0;
	point test_sam; vector<point> test_sams; int test_sams_size = 0;
	vector<double> X, W; double h;
	bool found; int found_count = 0; double PLAaccuracy; vector<double> PLAaccuracies;
	vector<result> pla_results; int pla_results_size = 0;
	for (int i = 0; i < random_splits * 2; i += 2)
	{
		/* 4.1 Convert training set to correct format for neural network. */
		for (int j = 0; j < training_set_size; j++)
		{
			training_sam.X1 = plaCV[i][j][0];
			training_sam.X2 = plaCV[i][j][1];
			training_sam.T = plaCV[i][j][2];
			training_sams.push_back(training_sam);
			training_sams_size++;
		}
		/* 4.2 Create the neural network. */
		cout << "Creating PLA neural network #" << nn_count << " of " << random_splits << "... ";
		PLAresult = neural_network(training_sams, training_set_size);
		pla_results.push_back(PLAresult); pla_results_size++;
		cout << "DONE" << endl; cout << "" << endl;
		found_count = 0;
		/* 4.3 Convert test set to correct format for network search. */
		cout << "Checking test set and counting successfully found samples in PLA neural network #" << nn_count << "... ";
		for (int k = 0; k < test_set_size; k++)
		{
			X = { plaCV[i+1][k][0], plaCV[i+1][k][1] };
			W = { PLAresult.W0, PLAresult.W1, PLAresult.W2 };
			h = perceptron(X, W);
			test_sam = sigmoid(h, X);
			test_sams.push_back(test_sam);
			test_sams_size++;
			found = search(test_sam, PLAresult);
			if (found == true)
			{
				found_count++;
			}
		}
		cout << "DONE" << endl; cout << "" << endl;
		double PLAfound_count = found_count;
		double PLAtest_set_size = test_set_size;
		PLAaccuracy = PLAfound_count / PLAtest_set_size;
		PLAaccuracies.push_back(PLAaccuracy);
		cout << "The accuracy result of neural network #" << nn_count << " has been saved." << endl;
		cout << "" << endl;
		test_sams = {}; test_sams_size = 0;
		nn_count++;
	}
	graphs(pla_results, pla_results_size, "PLAresults1.txt");
	cout << "PLA graph results have been saved to file." << endl; cout << "" << endl;

	/* 5. Display the results. */
	ofstream PLAresults("PLAresults2.txt");
	if (PLAresults.is_open())
	{
		PLAresults << "     PLA Cross Validation Result"      << endl;
		PLAresults << "*************************************" << endl;
		PLAresults << "Samples = " << samples_count << endl;
		PLAresults << "Cross Validation" << endl;
		PLAresults << "  # of Random Splits = " << random_splits << endl; int trainP = training_portion * 100;
		PLAresults << "  Training Set % of Samples = " << trainP << "% = " << training_set_size << endl;
		PLAresults << "  Test Set % of Samples = " << 100 - trainP << "% = " << test_set_size << endl;
		PLAresults << "+++++++++++++++++++++++++++++++++++++" << endl;
		PLAresults << ""                                      << endl;
		PLAresults << "           Accuracy Results"           << endl;
		PLAresults << "-------------------------------------" << endl;
		PLAresults << "  Test Run     Accuracy on Test Set" << endl;
		PLAresults << "-------------------------------------" << endl;
		double accuracy;
		for (int i = 0; i < random_splits; i++)
		{
			PLAresults << setw(7) << i + 1;
			accuracy = PLAaccuracies[i] * 100;
			PLAresults << setw(20) << setprecision(4) << accuracy << "%" << endl;
		}
		PLAresults << "+++++++++++++++++++++++++++++++++++++" << endl;
		PLAresults.close();
		cout << "PLA accuracy results have been saved to file." << endl; cout << "" << endl;
	}

	/* 6. Create neural networks from logic gates. */
	sample logic_sample; vector<sample> logic_samples;
	for (int i = 0; i < 4; i++)
	{
		if (i == 0)
		{
			logic_sample.X1 = 0;
			logic_sample.X2 = 0;
		}
		else if (i == 1)
		{
			logic_sample.X1 = 0;
			logic_sample.X2 = 1;
		}
		else if (i == 2)
		{
			logic_sample.X1 = 1;
			logic_sample.X2 = 0;
		}
		else
		{
			logic_sample.X1 = 1;
			logic_sample.X2 = 1;
		}
		logic_samples.push_back(logic_sample);
	}
	sample logic_sam;
	point logic_point;
	vector<result> logic_results; int logic_results_size = 0;

	/* 6.1 AND Logic Gate */
	vector<sample> AND_logic_samples;
	for (int i = 0; i < 4; i++)
	{
		if (logic_samples[i].X1 == 1 && logic_samples[i].X2 == 1)
			logic_samples[i].T = 1;
		else
			logic_samples[i].T = 0;
	}
	AND_logic_samples = logic_samples;
	cout << "Creating neural network for AND gate... ";
	result ANDresult = neural_network(AND_logic_samples, 4);
	logic_results.push_back(ANDresult);
	logic_results_size++;
	cout << "DONE" << endl; cout << "" << endl;
	
	/* 6.2 OR Logic Gate */
	vector<sample> OR_logic_samples;
	for (int i = 0; i < 4; i++)
	{
		if (logic_samples[i].X1 == 0 && logic_samples[i].X2 == 0)
			logic_samples[i].T = 0;
		else
			logic_samples[i].T = 1;
	}
	OR_logic_samples = logic_samples;
	cout << "Creating neural network for OR gate... ";
	result ORresult = neural_network(OR_logic_samples, 4);
	logic_results.push_back(ORresult);
	logic_results_size++;
	cout << "DONE" << endl; cout << "" << endl;

	/* 6.3 NAND Logic Gate */
	vector<sample> NAND_logic_samples;
	for (int i = 0; i < 4; i++)
	{
		if (logic_samples[i].X1 == 1 && logic_samples[i].X2 == 1)
			logic_samples[i].T = 0;
		else
			logic_samples[i].T = 1;
	}
	NAND_logic_samples = logic_samples;
	cout << "Creating neural network for NAND gate... ";
	result NANDresult = neural_network(NAND_logic_samples, 4);
	logic_results.push_back(NANDresult);
	logic_results_size++;
	cout << "DONE" << endl; cout << "" << endl;
	
	/* 6.4 NOR Logic Gate */
	vector<sample> NOR_logic_samples;
	for (int i = 0; i < 4; i++)
	{
		if (logic_samples[i].X1 == 0 && logic_samples[i].X2 == 0)
			logic_samples[i].T = 1;
		else
			logic_samples[i].T = 0;
	}
	NOR_logic_samples = logic_samples;
	cout << "Creating neural network for NOR gate... ";
	result NORresult = neural_network(NOR_logic_samples, 4);
	logic_results.push_back(NORresult);
	logic_results_size++;
	cout << "DONE" << endl; cout << "" << endl;

	/* 6.5 XOR Logic Gate */
	vector<sample> XOR_logic_samples;
	for (int i = 0; i < 4; i++)
	{
		if (logic_samples[i].X1 == 0 && logic_samples[i].X2 == 0 || logic_samples[i].X1 == 1 && logic_samples[i].X2 == 1)
			logic_samples[i].T = 0;
		else
			logic_samples[i].T = 1;
	}
	XOR_logic_samples = logic_samples;
	cout << "Creating neural network for XOR gate... ";
	result XORresult = neural_network(XOR_logic_samples, 4);
	logic_results.push_back(XORresult);
	logic_results_size++;
	cout << "DONE" << endl; cout << "" << endl;

	graphs(logic_results, logic_results_size, "LOGresults.txt");
	cout << "Logic graph data has been saved to file." << endl; cout << "" << endl;

	//system("pause");
	return 0;
}
