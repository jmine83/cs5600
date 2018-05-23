// 09/30/2016 - CS 5600 - Meine, Joel
// Homework #2 - Decision Tree

#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>
#include <fstream>
#include <algorithm>
using namespace std;

/* Round double to a specified decimal precision.  */
double round_nplaces(double &value, const uint32_t &to)
{
	uint32_t places = 1, whole = *(&value);
	for (uint32_t i = 0; i < to; i++) places *= 10;
	value -= whole;
	value *= places;
	value = round(value);
	value /= places;
	value += whole;
	return(value);
}

/* The entropy of an attribute. */
int eO_error = 0;
int eI_error = 0;
int samples_error = 0;
vector<vector<double>> entropy(vector<int> input, vector<int> output, int symbols, int samples)
{
	/* Calculate entropy of output. */
	double eO = 0; /* The entropy of output. */
	int sum1 = 0, sum0 = 0; /* Count of 1s and 0s in output. */
	for (int o = 0; o < samples; o++)
	{
		if (output[o] == 1)
			sum1++;
		else if (output[o] == 0)
			sum0++;
		else
			eO_error++;
	}
	double s1, s0;
	s1 = sum1; s0 = sum0;
	/* Probability of 1s and 0s in output. */
	if (samples == 0)
		samples_error++;
	double p1 = s1 / samples;
	double p0 = s0 / samples;
	if (p1 != 0)
	{
		double e1 = -p1*log2(p1);
		eO = eO + e1;
	}
	if (p0 != 0)
	{
		double e0 = -p0*log2(p0);
		eO = eO + e0;
	}

	/* Calculate entropy of input. */
	vector<double> eI = {}; /* The entropy of input. */
	vector<int> symbol = {}, sum = {}, sum11 = {}, sum00 = {};
	for (int i = 0; i < symbols; i++)
	{
		eI.push_back(0); symbol.push_back(i); sum.push_back(0); sum11.push_back(0); sum00.push_back(0);
	}
	for (int i = 0; i < symbols; i++)
	{
		for (int ii = 0; ii < samples; ii++)
		{
			/* Count of instances of each symbol. */
			if (input[ii] == symbol[i])
			{
				sum[i] = sum[i] + 1;
				/* Count of instances to each output value for symbol. */
				if (output[ii] == 1)
					sum11[i] = sum11[i] + 1;
				else if (output[ii] == 0)
					sum00[i] = sum00[i] + 1;
				else
					eI_error++;
			}
		}
	}
	double s = 0;
	vector<double> sum111 = {}, sum000 = {}, sumD = {};
	for (int i = 0; i < symbols; i++)
	{
		s = sum11[i];
		sum111.push_back(s);
		s = sum00[i];
		sum000.push_back(s);
		s = sum[i];
		sumD.push_back(s);
	}
	/* Probability of 1s and 0s in input for symbol. */
	double p11 = 0, p00 = 0;
	for (int i = 0; i < symbols; i++)
	{
		if (sum[i] != 0)
		{
			p11 = sum111[i] / sum[i];
			p00 = sum000[i] / sum[i];
			if (p11 != 0)
			{
				double e11 = -p11*log2(p11);
				eI[i] = eI[i] + e11;
			}
			if (p00 != 0)
			{
				double e00 = -p00*log2(p00);
				eI[i] = eI[i] + e00;
			}
		}
	}
	vector<vector<double>> e = {{eO},eI,sumD};
	return(e);
}

/* The information gain of an attribute. */
double info_gain(vector<vector<double>> e, int symbols, int samples)
{
	double root_term = e[0][0];
	double terminal_term = 0;
	vector<double> terminal_terms = {};
	for (int i = 0; i < symbols; i++)
	{
		terminal_term = (e[2][i]/samples)*e[1][i];
		terminal_terms.push_back(terminal_term);
	}
	double g = 0;
	for (int i = 0; i < symbols; i++)
	{
		if (i == 0)
			g = root_term - terminal_terms[i];
		else
			g = g - terminal_terms[i];
	}
	round_nplaces(g,6);
	return(g);
}

/* The information gain of highest value of available attributes. */
int best_attribute(vector<double> info_gains, vector<int> attrbiutes, int inputs)
{
	double max_gain_value = 0;
	int best_att;
	for (int i = 0; i < inputs; i++)
	{
		if (info_gains[i] > max_gain_value)
		{
			max_gain_value = info_gains[i];
			best_att = attrbiutes[i];
		}
	}
	return(best_att);
}

/* The remaining attributes after a selected attribute is removed. */
vector<vector<int>> remove_attribute(int attribute, vector<int> attributes, int attributes_count, vector<int> attributes_key)
{
	vector<vector<int>> remove_att;
	int attribute_symbols;
	for (int i = 0; i < attributes_count; i++)
	{
		if (attributes[i] == attribute)
		{
			attribute_symbols = i;
			attributes.erase(remove(attributes.begin(),attributes.end(),attribute),attributes.end());
			i = attributes_count;
		}
	}
	attributes_key.erase(attributes_key.begin()+attribute_symbols);
	remove_att = {attributes,attributes_key};
	return(remove_att);
}

/* The list of possible symbols of an attribute from the number of symbols. */
vector<int> symbols(int attribute, vector<int> attributes, int attributes_count, vector<int> attributes_key)
{
	int attribute_symbols;
	for (int i = 0; i < attributes_count; i++)
	{
		if (attributes[i] == attribute)
		{
			attribute_symbols = attributes_key[i];
			i = attributes_count;
		}
	}
	vector<int> ss;
	for (int i = 0; i < attribute_symbols; i++)
	{
		ss.push_back(i);
	}
	return(ss);
}

/* The internal node of a decision tree. */
typedef struct {
	int Attribute; /* The attribute of the internal node; i.e. node of symbol. */
	int Link; /* The parent attribute of the internal node. No_Parent=-1 */
	int Link_Symbol; /* The symbol of the parent attribute of the internal node. No_Parent=-1 */
	vector<int> Symbols; /* Each possible symbol of the attribute; i.e. attribute branches. */
	vector<vector<int>> Samples; /* The samples contained in each symbol of attribute. */
	int Samples_Count; /* The number of samples contained in the internal node. */
} Inode;

/* The split node of an internal node in a decision tree. */
typedef struct {
	vector<vector<int>> Samples; /* The samples contained in the split node. */
	int Samples_Count; /* The number of samples contained in the split node. */
} Snode;

/* The terminal node of a decision tree. */
typedef struct {
	int Attribute; /* The attribute of the terminal node. */
	int Symbol; /* The symbol of the terminal node. */
	bool Class; /* The class of terminal node. _ Yes=true No=false */
	vector<vector<int>> Samples; /* The classification of a sample being either true or false. */
	int Samples_Count; /* The number of samples contained in the terminal node. */
} Tnode;

/* The set of nodes of a decision tree. */
typedef struct {
	vector<Inode> Inodes; /* The internal nodes of the decision tree. */
	int Inodes_Count; /* The number of internal nodes of the decision tree. */
	vector<Tnode> Tnodes; /* The terminal nodes of the decision tree. */
	int Tnodes_Count; /* The number of terminal nodes of the decision tree. */
} DTnodes;

/* The decision tree. */
int value_error = 0;
bool attributes_empty = false;
int attribute; /* The last known best attribute selected. */
int symbol; /* The symbol of attribute currently being evaluated. */
vector<Inode> roots; /* The roots of the decision tree. */
vector<Tnode> leafs; /* The leafs of the decision tree. */
int roots_size = 0, leafs_size = 0;
void decision_tree(vector<vector<int>> samples, int samples_count, int samples_output, vector<int> attributes, int attributes_count, vector<int> attributes_key)
/*
samples = the remainder of available samples.
samples_count = the number of remaining samples.
samples_output = the position in the sample of output.
attributes = the remainder of available attributes.
attributes_count = the number of remaining attributes.
attributes_key = the number of possible symbols each remaining attribute can hold.
*/
{
	vector<int> entropy_input = {}, entropy_output = {};
	vector<vector<double>> entp; double infg; vector<double> infgs;
	int paths = 0;

	/* 0. Initialize the current nodes of the decision tree. */
	Inode root; Snode branch; Tnode leaf;
	root.Samples = samples; root.Samples_Count = samples_count;

	/* 1. Count the number of 1s and 0s in the output of each available sample. */
	int sum1 = 0, sum0 = 0;
	for (int i = 0; i < samples_count; i++)
	{
		if (samples[i][samples_output] == 1)
			sum1++;
		else if (samples[i][samples_output] == 0)
			sum0++;
		else
			value_error++;
	}
	/* 2. Set node to be leaf if either all 1s or all 0s were detected. */
	if (sum1 == samples_count || sum0 == samples_count)
	{
		leaf.Attribute = attribute;
		leaf.Symbol = symbol;
		if (sum1 == samples_count)
			leaf.Class = true;
		else if (sum0 == samples_count)
			leaf.Class = false;
		else
			value_error++;
		leaf.Samples_Count = 0;
		for (int i = 0; i < samples_count; i++)
		{
			leaf.Samples.push_back(samples[i]);
			leaf.Samples_Count++;
		}
		leafs.push_back(leaf);
		leafs_size++;
	}
	/* 3. If no attributes remain, then set based on the majority output. */
	else if (attributes_empty == true)
	{
		leaf.Attribute = attribute;
		leaf.Symbol = symbol;
		if (sum1 >= sum0)
			leaf.Class = true;
		else
			leaf.Class = false;
		leaf.Samples_Count = 0;
		for (int i = 0; i < samples_count; i++)
		{
			leaf.Samples.push_back(samples[i]);
			leaf.Samples_Count++;
		}
		leafs.push_back(leaf);
		leafs_size++;
	}
	else
	{
		/* 4. Determine the best attribute from the remaining attributes. */
		for (int i = 0; i < attributes_count; i++)
		{
			for (int j = 0; j < samples_count; j++)
			{
				entropy_input.push_back(samples[j][attributes[i]]);
				entropy_output.push_back(samples[j][samples_output]);
			}
			/* 4.1 Calculate entropy and information gain of currently selected attribute. */
			entp = entropy(entropy_input, entropy_output, attributes_key[i], samples_count);
			infg = info_gain(entp, attributes_key[i], samples_count);
			infgs.push_back(infg);
			entropy_input = {}; entropy_output = {};
		}
		/* 5. Assign best attribute available to the root node. */
		root.Attribute = best_attribute(infgs, attributes ,attributes_count);
		root.Symbols = symbols(root.Attribute,attributes,attributes_count,attributes_key);
		paths = root.Symbols.back()+1;
		/* 5.1 Create a new path to a new node for each possible symbol of attribute. */
		vector<Snode> Nodes; /* The available nodes that the internal node can link to. */
		for (int i = 0; i < paths; i++)
		{
			branch.Samples_Count = 0;
			Nodes.push_back(branch);
		}
		/* 6. Split the samples and store them in their respective node. */
		sum1 = 0, sum0 = 0;
		for (int i = 0; i < samples_count; i++)
		{
			if (samples[i][samples_output] == 1)
				sum1++;
			else if (samples[i][samples_output] == 0)
				sum0++;
			else
				value_error++;
			for (int j = 0; j < paths; j++)
			{
				/* Check if sample belongs in the loaded branch. */
				if (samples[i][root.Attribute] == j)
				{
					branch = Nodes[j]; /* Load branch to receive a sample. */
					branch.Samples.push_back(samples[i]); /* Add sample to loaded branch. */
					branch.Samples_Count++; /* Increment the sample count of loaded branch. */
					Nodes[j] = branch; /* Save branch with the sampled added.  */
					j = paths; /* Check the next sample. */
				}
			}
		}
		/* 7. Check if branch has no samples. */
		for (int i = 0; i < paths; i++)
		{
			branch = Nodes[i]; /* Load branch to check if it has no samples. */
			/* For example, an attribute may support three possible values, but the available
			samples may have zero cases of one of those three possible values. */
			if (branch.Samples_Count == 0)
			{
				leaf.Attribute = root.Attribute;
				leaf.Symbol = i;
				if (sum1 >= sum0)
					leaf.Class = true;
				else
					leaf.Class = false;
				leaf.Samples_Count = 0;
				leaf.Samples = {};
				leafs.push_back(leaf);
				leafs_size++;
			}
		}
		/* 8. Add root node to decision tree. */
		if (roots_size == 0)
		{
			root.Link = -1; root.Link_Symbol = -1;
		}
		else
		{
			root.Link = roots.back().Attribute;
			root.Link_Symbol = symbol;
		}
		roots.push_back(root);
		roots_size++;
		/* 9. Remove current attribute and create a subtree for each branch. */
		vector<vector<int>> atts;
		atts = remove_attribute(root.Attribute,attributes,attributes_count,attributes_key);
		attributes = atts[0]; attributes_key = atts[1];
		attributes_count--;
		if (attributes_count == 0)
			attributes_empty = true;
		for (int k = 0; k < paths; k++)
		{
			branch = Nodes[k]; /* Load branch to create subtree. */
			samples = branch.Samples;
			samples_count = branch.Samples_Count;
			attribute = root.Attribute;
			symbol = k;
			if (samples_count != 0)
				decision_tree(samples, samples_count, samples_output, attributes, attributes_count, attributes_key);
		}
	}
}

/* The decision tree is reset. */
void reset_tree()
{
	roots = {}; roots_size = 0; leafs = {}; leafs_size = 0;
	eO_error = 0; eI_error = 0; samples_error = 0; value_error = 0;
}

/* The result of a sample search in a decision tree. */
typedef struct {
	bool Found; /* Does the sample fit into the decision tree? _ Yes=true No=false */
	Tnode Location; /* The location of the sample if located in the decision tree. */
} search_result;

search_result search_tree(vector<int> sample, int sample_output, DTnodes tree)
{
	int attribute, symbol; bool type; search_result result;
	Tnode not_found; not_found.Attribute = -1; not_found.Symbol = -1;
	for (int i = 0; i < tree.Inodes_Count; i++)
	{
		/* The search begins by checking each internal node of the decision tree. */
		attribute = tree.Inodes[i].Attribute; /* Load the attribute being checked. */
		symbol = sample[attribute]; /* Load the symbol from sample at selected attribute. */
		for (int j = 0; j < tree.Tnodes_Count; j++)
		{
			/* Then search all terminal nodes of the decision tree. */
			if (tree.Tnodes[j].Attribute == attribute && tree.Tnodes[j].Symbol == symbol)
			{
				/* A matching terminal node is found. */
				type = sample[sample_output];
				if (tree.Tnodes[j].Class == type)
				{
					/* The sample does fit into the decision tree. */
					result.Found = true;
					result.Location = tree.Tnodes[j];
					i = tree.Inodes_Count; j = tree.Tnodes_Count;
				}
				else
				{
					/* The sample does not fit into the decision tree. */
					result.Found = false;
					result.Location = not_found;
					i = tree.Inodes_Count; j = tree.Tnodes_Count;
				}
			}
		}
	}
	return(result);
}

/* The cross validation of a list of samples. */
int training_set_size = 0, test_set_size = 0;
vector<vector<vector<int>>> cross_validation(vector<vector<int>> samples, int samples_count, int random_splits, double training_portion)
{
	vector<vector<vector<int>>> validation_set = {};
	vector<vector<int>> training_set = {};
	vector<vector<int>> test_set = {};
	int training = round(training_portion*samples_count);
	int test = round((1-training_portion)*samples_count);
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
	/* 1. Initialize the training set of samples. */
	/* 1.1 Restaurant Table Waiting */
	cout << "Initializing training set for RTW... ";
	int RTWsamples = 12; /* Number of RTW training samples. */
	int RTWinputs = 10; /* Number of RTW input attributes. */
	vector<int> RTWattributes = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	vector<int> RTWkey = { 2, 2, 2, 2, 3, 3, 2, 2, 4, 4 };
	vector<int> x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12;
		/*
		Alt: Is there a sutiable alternative restaurant nearby? _ Yes=1 No=0
		Bar: Does the restaurant have a comfortable bar area to wait in? _ Yes=1 No=0
		Fri: Is it a Friday or Saturday? _ Yes=1 No=0
		Hun: Is the patron hungry? _ Yes=1 No=0
		Pat; How many people are in the restaurant? _ None=0 Some=1 Full=2
		Price: What is the restaurant price range? _ $=0 $$=1 $$$=2
		Rain: Is it raining outside? _ Yes=1 No=0
		Res: Was a reservation made? _ Yes=1 No=0
		Type: What kind of restaurant is it? _ French=0 Italian=1 Thai=2 Burger=3
		Est: What is the estimated wait time (in minutes)? _ 0-10=0 10-30=1 30-60=2 >60=3
		WillWait: Will patron wait for a table? _ Yes=1 No=0
		*/
	// = { Alt, Bar, Fri, Hun, Pat, Price, Rain, Res, Type, Est, WillWait }
	x1 = { 1, 0, 0, 1, 1, 2, 0, 1, 0, 0, 1 };
	x2 = { 1, 0, 0, 1, 2, 0, 0, 0, 2, 2, 0 };
	x3 = { 0, 1, 0, 0, 1, 0, 0, 0, 3, 0, 1 };
	x4 = { 1, 0, 1, 1, 2, 0, 1, 0, 2, 1, 1 };
	x5 = { 1, 0, 1, 0, 2, 2, 0, 1, 0, 3, 0 };
	x6 = { 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1 };
	x7 = { 0, 1, 0, 0, 0, 0, 1, 0, 3, 0, 0 };
	x8 = { 0, 0, 0, 1, 1, 1, 1, 1, 2, 0, 1 };
	x9 = { 0, 1, 1, 0, 2, 0, 1, 0, 3, 3, 0 };
	x10 = { 1, 1, 1, 1, 2, 2, 0, 1, 1, 1, 0 };
	x11 = { 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0 };
	x12 = { 1, 1, 1, 1, 2, 0, 0, 0, 3, 2, 1 };
	vector<vector<int>> RTWtraining_set = { x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12 };
	cout << "DONE" << endl; cout << "" << endl;
	
	/* 1.2 HIV-1 Schilling Data */
	cout << "Initializing data set for HIV... ";
	int HIVsamples = 0; /* Number of HIV training samples counted. */
	int HIVinputs = 8; /* Number of HIV input attributes. */
		/*
		aa1, aa2, aa3, aa4, aa5, aa6, aa7, aa8: The amino acid sequence of the sample octamer.
		_ Each amino acid can be one of 20 possible values.
		Cleaves: Does the HIV-1 protease cleave? _ Yes=1 No=0
		*/
	vector<int> HIVattributes = { 0, 1, 2, 3, 4, 5, 6, 7 };
	vector<int> HIVkey = { 20, 20, 20, 20, 20, 20, 20, 20 };
	vector<int> sample = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	vector<int> buffer = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	vector<vector<int>> HIVdata_set = {};
	ifstream dataset("schillingData.txt");
	char c; int i = 0, I = 0; bool negative = false;
	while (dataset.get(c))
	{
		/* Read in the sample data and convert to sample vectors. */
		if (i <= 7)
		{
			if (c == 'A')
				buffer[i] = 0;
			else if (c == 'R')
				buffer[i] = 1;
			else if (c == 'N')
				buffer[i] = 2;
			else if (c == 'D')
				buffer[i] = 3;
			else if (c == 'C')
				buffer[i] = 4;
			else if (c == 'Q')
				buffer[i] = 5;
			else if (c == 'E')
				buffer[i] = 6;
			else if (c == 'G')
				buffer[i] = 7;
			else if (c == 'H')
				buffer[i] = 8;
			else if (c == 'I')
				buffer[i] = 9;
			else if (c == 'L')
				buffer[i] = 10;
			else if (c == 'K')
				buffer[i] = 11;
			else if (c == 'M')
				buffer[i] = 12;
			else if (c == 'F')
				buffer[i] = 13;
			else if (c == 'P')
				buffer[i] = 14;
			else if (c == 'S')
				buffer[i] = 15;
			else if (c == 'T')
				buffer[i] = 16;
			else if (c == 'W')
				buffer[i] = 17;
			else if (c == 'Y')
				buffer[i] = 18;
			else if (c == 'V')
				buffer[i] = 19;
			else
				buffer[i] = 20;
		}
		else
		{
			if (c == ',')
				;
			else if (c == '-')
				negative = true;
			else if (c == '1')
			{
				if (negative == false)
					buffer[8] = 1;
				else
					buffer[8] = 0;
			}
			else if (c == '\n')
			{
				sample = buffer;
				HIVdata_set.push_back(sample);
				HIVsamples++;
				buffer = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
				I = 1;
				negative = false;
				if (dataset.eof())
					goto done;
			}
			else
				;
		}
		if (I == 1)
		{
			i = 0; I = 0;
		}
		else
			i++;
	}
done: dataset.close();
	cout << "DONE" << endl; cout << "" << endl;

	/* 2. Create the decision tree. */
	/* 2.1 Restaurant Table Waiting */
	cout << "Creating decision tree for RTW... ";
	decision_tree(RTWtraining_set, RTWsamples, RTWinputs, RTWattributes, RTWinputs, RTWkey);
	vector<int> RTWerrors = { eO_error, eI_error, samples_error, value_error };
	DTnodes RTWdecision_tree;
	RTWdecision_tree.Inodes = roots;
	RTWdecision_tree.Inodes_Count = roots_size;
	RTWdecision_tree.Tnodes = leafs;
	RTWdecision_tree.Tnodes_Count = leafs_size;
	reset_tree();
	cout << "DONE" << endl;
	if (RTWerrors[0] == 0 && RTWerrors[1] == 0 && RTWerrors[2] == 0 && RTWerrors[3] == 0)
		cout << "No detected errors in RTW." << endl;
	else
		cout << "ERROR in RTW!" << endl;
	cout << "" << endl;

	/* 2.2 HIV-1 Schilling Data */
	cout << "Creating multiple training/test sets of randomly shuffled samples for HIV... ";
	int random_splits = 10; double training_portion = 0.8; int tree_count = 1;
	vector<vector<vector<int>>> hivCV = cross_validation(HIVdata_set, HIVsamples, random_splits, training_portion);
	cout << "DONE" << endl; cout << "" << endl;
	int HIVtraining_set_size = training_set_size; /* The number of samples contained in each training set. */
	int HIVtest_set_size = test_set_size; /* The number of samples contained in each test set. */
	vector<vector<int>> HIVerrors; /* The error count in each decision tree created. */
	search_result HIVsearch_result; /* The result of a sample search in a decision tree. */
	int HIVfound_count = 0; /* The number of samples in test set found in the decision tree. */
	double HIVaccuracy; /* The percentage of samples successfully found in the decision tree. */
	double HIVfound, HIVtest;
	vector<double> HIVaccuracies; /* The percentage of accuracy in each decision tree created. */
	for (int i = 0; i < random_splits*2; i+=2)
	{
		/* 2.2.1 Create a decision tree from a dataset that was randomly shuffled. */
		cout << "Creating HIV decision tree #" << tree_count << " of " << random_splits << "... ";
		decision_tree(hivCV[i], HIVtraining_set_size, HIVinputs, HIVattributes, HIVinputs, HIVkey);
		HIVerrors.push_back({ eO_error, eI_error, samples_error, value_error });
		DTnodes HIVdecision_tree;
		HIVdecision_tree.Inodes = roots;
		HIVdecision_tree.Inodes_Count = roots_size;
		HIVdecision_tree.Tnodes = leafs;
		HIVdecision_tree.Tnodes_Count = leafs_size;
		reset_tree();
		cout << "DONE" << endl;
		if (HIVerrors[tree_count-1][0] == 0 && HIVerrors[tree_count-1][1] == 0 && HIVerrors[tree_count-1][2] == 0 && HIVerrors[tree_count-1][3] == 0)
			cout << "No detected errors in HIV." << endl;
		else
			cout << "ERROR in HIV!" << endl;
		cout << "" << endl;
		HIVfound_count = 0;
		/* 2.2.2 Search every sample from test set in the decision tree. */
		cout << "Checking test set and counting successfully found samples in HIV decision tree #" << tree_count << "... ";
		int marker = 0; double percent = 0.10; int progress = 0;
		for (int j = 0; j < HIVtest_set_size; j++)
		{
			marker = percent*HIVtest_set_size;
			if (j == marker)
			{
				progress = percent*100;
				cout << progress << "% ";
				percent = percent + 0.10;
			}
			HIVsearch_result = search_tree(hivCV[i+1][j],HIVinputs,HIVdecision_tree);
			/* Count the number of samples successfully found in the decision tree. */
			if (HIVsearch_result.Found == true)
				HIVfound_count++;
		}
		cout << "DONE" << endl; cout << "" << endl;
		/* 2.2.3 Calculate the percentage of accuracy of the decision tree. */
		HIVfound = HIVfound_count; HIVtest = HIVtest_set_size;
		HIVaccuracy = HIVfound / HIVtest;
		HIVaccuracies.push_back(HIVaccuracy);
		cout << "The accuracy result of decision tree #" << tree_count << " has been saved." << endl;
		cout << "" << endl;
		tree_count++;
	}

	/* 3. Display the results. */
	/* 3.1 Restaurant Table Waiting */
	ofstream RTWresults("RTWresults.txt");
	if (RTWresults.is_open())
	{
		RTWresults << "      Restaurant Table Waiting"        << endl;
		RTWresults << "*************************************" << endl;
		RTWresults << "Alt: Is there a sutiable alternative restaurant nearby? _ Yes or No" << endl;
		RTWresults << "Bar: Does the restaurant have a comfortable bar area to wait in? _ Yes or No" << endl;
		RTWresults << "Fri: Is it a Friday or Saturda ? _ Yes or No" << endl;
		RTWresults << "Hun: Is the patron hungry? _ Yes or No" << endl;
		RTWresults << "Pat: How many people are in the restaurant? _ None, Some, Full" << endl;
		RTWresults << "Price: What is the restaurant price range? _ $, $$, $$$" << endl;
		RTWresults << "Rain: Is it raining outside? _ Yes or No" << endl;
		RTWresults << "Res: Was a reservation made? _ Yes or No" << endl;
		RTWresults << "Type: What kind of restaurant is it? _ French, Italian, Thai, Burger" << endl;
		RTWresults << "Est: What is the estimated wait time (in minutes)? _ 0-10, 10-30, 30-60, >60" << endl;
		RTWresults << "WillWait: Will patron wait for a table? _ Yes or No" << endl;
		RTWresults << "*************************************" << endl;
		RTWresults << ""                                      << endl;
		RTWresults << "            Training Set"              << endl;
		RTWresults << "-------------------------------------" << endl;
		for (int i = 0; i < RTWsamples; i++)
		{
			RTWresults << "Sample #" << i+1 << endl;
			RTWresults << "Alt = ";
			if (RTWtraining_set[i][0] == 1)
				RTWresults << "Yes" << endl;
			else
				RTWresults << "No" << endl;
			RTWresults << "Bar = ";
			if (RTWtraining_set[i][1] == 1)
				RTWresults << "Yes" << endl;
			else
				RTWresults << "No" << endl;
			RTWresults << "Fri = ";
			if (RTWtraining_set[i][2] == 1)
				RTWresults << "Yes" << endl;
			else
				RTWresults << "No" << endl;
			RTWresults << "Hun = ";
			if (RTWtraining_set[i][3] == 1)
				RTWresults << "Yes" << endl;
			else
				RTWresults << "No" << endl;
			RTWresults << "Pat = ";
			if (RTWtraining_set[i][4] == 0)
				RTWresults << "None" << endl;
			else if (RTWtraining_set[i][4] == 1)
				RTWresults << "Some" << endl;
			else
				RTWresults << "Full" << endl;
			RTWresults << "Price = ";
			if (RTWtraining_set[i][5] == 0)
				RTWresults << "$" << endl;
			else if (RTWtraining_set[i][5] == 1)
				RTWresults << "$$" << endl;
			else
				RTWresults << "$$$" << endl;
			RTWresults << "Rain = ";
			if (RTWtraining_set[i][6] == 1)
				RTWresults << "Yes" << endl;
			else
				RTWresults << "No" << endl;
			RTWresults << "Res = ";
			if (RTWtraining_set[i][7] == 1)
				RTWresults << "Yes" << endl;
			else
				RTWresults << "No" << endl;
			RTWresults << "Type = ";
			if (RTWtraining_set[i][8] == 0)
				RTWresults << "French" << endl;
			else if (RTWtraining_set[i][8] == 1)
				RTWresults << "Italian" << endl;
			else if (RTWtraining_set[i][8] == 2)
				RTWresults << "Thai" << endl;
			else
				RTWresults << "Burger" << endl;
			RTWresults << "Est = ";
			if (RTWtraining_set[i][9] == 0)
				RTWresults << "0-10" << endl;
			else if (RTWtraining_set[i][9] == 1)
				RTWresults << "10-30" << endl;
			else if (RTWtraining_set[i][9] == 2)
				RTWresults << "30-60" << endl;
			else
				RTWresults << ">60" << endl;
			RTWresults << "WillWait = ";
			if (RTWtraining_set[i][10] == 1)
				RTWresults << "Yes" << endl;
			else
				RTWresults << "No" << endl;
			if (i != RTWsamples-1)
				RTWresults << "" << endl;
		}
		RTWresults << "+++++++++++++++++++++++++++++++++++++" << endl;
		RTWresults << "            Decision Tree"             << endl;
		RTWresults << "-------------------------------------" << endl;
		for (int i = 0; i < RTWdecision_tree.Inodes_Count; i++)
		{
			RTWresults << "Root = ";
			if (RTWdecision_tree.Inodes[i].Attribute == 0)
				RTWresults << "Alt" << endl;
			else if (RTWdecision_tree.Inodes[i].Attribute == 1)
				RTWresults << "Bar" << endl;
			else if (RTWdecision_tree.Inodes[i].Attribute == 2)
				RTWresults << "Fri" << endl;
			else if (RTWdecision_tree.Inodes[i].Attribute == 3)
				RTWresults << "Hun" << endl;
			else if (RTWdecision_tree.Inodes[i].Attribute == 4)
				RTWresults << "Pat" << endl;
			else if (RTWdecision_tree.Inodes[i].Attribute == 5)
				RTWresults << "Price" << endl;
			else if (RTWdecision_tree.Inodes[i].Attribute == 6)
				RTWresults << "Rain" << endl;
			else if (RTWdecision_tree.Inodes[i].Attribute == 7)
				RTWresults << "Res" << endl;
			else if (RTWdecision_tree.Inodes[i].Attribute == 8)
				RTWresults << "Type" << endl;
			else if (RTWdecision_tree.Inodes[i].Attribute == 9)
				RTWresults << "Est" << endl;

			RTWresults << "Branch of ";
			if (RTWdecision_tree.Inodes[i].Link == -1)
				RTWresults << "NULL";
			else if (RTWdecision_tree.Inodes[i].Link == 0)
				RTWresults << "Alt";
			else if (RTWdecision_tree.Inodes[i].Link == 1)
				RTWresults << "Bar";
			else if (RTWdecision_tree.Inodes[i].Link == 2)
				RTWresults << "Fri";
			else if (RTWdecision_tree.Inodes[i].Link == 3)
				RTWresults << "Hun";
			else if (RTWdecision_tree.Inodes[i].Link == 4)
				RTWresults << "Pat";
			else if (RTWdecision_tree.Inodes[i].Link == 5)
				RTWresults << "Price";
			else if (RTWdecision_tree.Inodes[i].Link == 6)
				RTWresults << "Rain";
			else if (RTWdecision_tree.Inodes[i].Link == 7)
				RTWresults << "Res";
			else if (RTWdecision_tree.Inodes[i].Link == 8)
				RTWresults << "Type";
			else if (RTWdecision_tree.Inodes[i].Link == 9)
				RTWresults << "Est";

			RTWresults << " on ";
			if (RTWdecision_tree.Inodes[i].Link == -1)
				RTWresults << "NULL" << endl;
			else if (RTWdecision_tree.Inodes[i].Link == 0)
			{
				if (RTWdecision_tree.Inodes[i].Link_Symbol == 1)
					RTWresults << "Yes" << endl;
				else
					RTWresults << "No" << endl;
			}
			else if (RTWdecision_tree.Inodes[i].Link == 1)
			{
				if (RTWdecision_tree.Inodes[i].Link_Symbol == 1)
					RTWresults << "Yes" << endl;
				else
					RTWresults << "No" << endl;
			}
			else if (RTWdecision_tree.Inodes[i].Link == 2)
			{
				if (RTWdecision_tree.Inodes[i].Link_Symbol == 1)
					RTWresults << "Yes" << endl;
				else
					RTWresults << "No" << endl;
			}
			else if (RTWdecision_tree.Inodes[i].Link == 3)
			{
				if (RTWdecision_tree.Inodes[i].Link_Symbol == 1)
					RTWresults << "Yes" << endl;
				else
					RTWresults << "No" << endl;
			}
			else if (RTWdecision_tree.Inodes[i].Link == 4)
			{
				if (RTWdecision_tree.Inodes[i].Link_Symbol == 0)
					RTWresults << "None" << endl;
				else if (RTWdecision_tree.Inodes[i].Link_Symbol == 1)
					RTWresults << "Some" << endl;
				else
					RTWresults << "Full" << endl;
			}
			else if (RTWdecision_tree.Inodes[i].Link == 5)
			{
				if (RTWdecision_tree.Inodes[i].Link_Symbol == 0)
					RTWresults << "$" << endl;
				else if (RTWdecision_tree.Inodes[i].Link_Symbol == 1)
					RTWresults << "$$" << endl;
				else
					RTWresults << "$$$" << endl;
			}
			else if (RTWdecision_tree.Inodes[i].Link == 6)
			{
				if (RTWdecision_tree.Inodes[i].Link_Symbol == 1)
					RTWresults << "Yes" << endl;
				else
					RTWresults << "No" << endl;
			}
			else if (RTWdecision_tree.Inodes[i].Link == 7)
			{
				if (RTWdecision_tree.Inodes[i].Link_Symbol == 1)
					RTWresults << "Yes" << endl;
				else
					RTWresults << "No" << endl;
			}
			else if (RTWdecision_tree.Inodes[i].Link == 8)
			{
				if (RTWdecision_tree.Inodes[i].Link_Symbol == 0)
					RTWresults << "French" << endl;
				else if (RTWdecision_tree.Inodes[i].Link_Symbol == 1)
					RTWresults << "Italian" << endl;
				else if (RTWdecision_tree.Inodes[i].Link_Symbol == 2)
					RTWresults << "Thai" << endl;
				else
					RTWresults << "Burger" << endl;
			}
			else if (RTWdecision_tree.Inodes[i].Link == 9)
			{
				if (RTWdecision_tree.Inodes[i].Link_Symbol == 0)
					RTWresults << "0-10" << endl;
				else if (RTWdecision_tree.Inodes[i].Link_Symbol == 1)
					RTWresults << "10-30" << endl;
				else if (RTWdecision_tree.Inodes[i].Link_Symbol == 2)
					RTWresults << "30-60" << endl;
				else
					RTWresults << ">60" << endl;
			}
			for (int j = 0; j < RTWdecision_tree.Tnodes_Count; j++)
			{
				if (RTWdecision_tree.Inodes[i].Attribute == RTWdecision_tree.Tnodes[j].Attribute)
				{
					RTWresults << "  Leaf = ";
					if (RTWdecision_tree.Inodes[i].Attribute == 0)
					{
						if (RTWdecision_tree.Tnodes[j].Symbol == 1)
							RTWresults << "Yes";
						else
							RTWresults << "No";
					}
					else if (RTWdecision_tree.Inodes[i].Attribute == 1)
					{
						if (RTWdecision_tree.Tnodes[j].Symbol == 1)
							RTWresults << "Yes";
						else
							RTWresults << "No";
					}
					else if (RTWdecision_tree.Inodes[i].Attribute == 2)
					{
						if (RTWdecision_tree.Tnodes[j].Symbol == 1)
							RTWresults << "Yes";
						else
							RTWresults << "No";
					}
					else if (RTWdecision_tree.Inodes[i].Attribute == 3)
					{
						if (RTWdecision_tree.Tnodes[j].Symbol == 1)
							RTWresults << "Yes";
						else
							RTWresults << "No";
					}
					else if (RTWdecision_tree.Inodes[i].Attribute == 4)
					{
						if (RTWdecision_tree.Tnodes[j].Symbol == 0)
							RTWresults << "None";
						else if (RTWdecision_tree.Tnodes[j].Symbol == 1)
							RTWresults << "Some";
						else
							RTWresults << "Full";
					}
					else if (RTWdecision_tree.Inodes[i].Attribute == 5)
					{
						if (RTWdecision_tree.Tnodes[j].Symbol == 0)
							RTWresults << "$";
						else if (RTWdecision_tree.Tnodes[j].Symbol == 1)
							RTWresults << "$$";
						else
							RTWresults << "$$$";
					}
					else if (RTWdecision_tree.Inodes[i].Attribute == 6)
					{
						if (RTWdecision_tree.Tnodes[j].Symbol == 1)
							RTWresults << "Yes";
						else
							RTWresults << "No";
					}
					else if (RTWdecision_tree.Inodes[i].Attribute == 7)
					{
						if (RTWdecision_tree.Tnodes[j].Symbol == 1)
							RTWresults << "Yes";
						else
							RTWresults << "No";
					}
					else if (RTWdecision_tree.Inodes[i].Attribute == 8)
					{
						if (RTWdecision_tree.Tnodes[j].Symbol == 0)
							RTWresults << "French";
						else if (RTWdecision_tree.Tnodes[j].Symbol == 1)
							RTWresults << "Italian";
						else if (RTWdecision_tree.Tnodes[j].Symbol == 2)
							RTWresults << "Thai";
						else
							RTWresults << "Burger";
					}
					else if (RTWdecision_tree.Inodes[i].Attribute == 9)
					{
						if (RTWdecision_tree.Tnodes[j].Symbol == 0)
							RTWresults << "0-10";
						else if (RTWdecision_tree.Tnodes[j].Symbol == 1)
							RTWresults << "10-30";
						else if (RTWdecision_tree.Tnodes[j].Symbol == 2)
							RTWresults << "30-60";
						else
							RTWresults << ">60";
					}

					RTWresults << "; Class = ";
					if (RTWdecision_tree.Tnodes[j].Class == true)
						RTWresults << "Yes";
					else
						RTWresults << "No";
					
					RTWresults << "; Sample #s = ";
					for (int k = 0; k < RTWsamples; k++)
					{
						for (int l = 0; l < RTWdecision_tree.Tnodes[j].Samples_Count; l++)
						{
							if (RTWtraining_set[k] == RTWdecision_tree.Tnodes[j].Samples[l])
								RTWresults << k + 1 << " ";
						}
					}
					RTWresults << "" << endl;
				}
			}
			if (i != RTWdecision_tree.Inodes_Count-1)
				RTWresults << "" << endl;
		}
		RTWresults << "+++++++++++++++++++++++++++++++++++++" << endl;
		RTWresults.close();
		cout << "RTW results have been saved to file." << endl; cout << "" << endl;
	}
	
	/* 3.2 HIV-1 Schilling Data */
	ofstream HIVresults("HIVresults.txt");
	if (HIVresults.is_open())
	{
		HIVresults << "        HIV-1 Schilling Data"          << endl;
		HIVresults << "*************************************" << endl;
		HIVresults << "aa1, aa2, aa3, aa4, aa5, aa6, aa7, aa8: The amino acid sequence of the sample octamer." << endl;
		HIVresults << ">> Each amino acid can be one of 20 possible values." << endl;
		HIVresults << "Cleaves: Does the HIV-1 protease cleave? _ Yes or No" << endl;
		HIVresults << "*************************************" << endl;
		HIVresults << ""                                      << endl;
		HIVresults << "              Data Set"                << endl;
		HIVresults << "-------------------------------------" << endl;
		HIVresults << "Samples = " << HIVsamples << endl;
		HIVresults << "Cross Validation" << endl;
		HIVresults << "  # of Random Splits = " << random_splits << endl; int trainP = training_portion*100;
		HIVresults << "  Training Set % of Samples = " << trainP << "% = " << HIVtraining_set_size << endl;
		HIVresults << "  Test Set % of Samples = " << 100-trainP << "% = " << HIVtest_set_size << endl;
		HIVresults << "+++++++++++++++++++++++++++++++++++++" << endl;
		HIVresults << ""                                      << endl;
		HIVresults << "           Accuracy Results"           << endl;
		HIVresults << "-------------------------------------" << endl;
		HIVresults << "  Test Run     Accuracy on Test Set"   << endl;
		HIVresults << "-------------------------------------" << endl;
		double accuracy;
		for (int i = 0; i < random_splits; i++)
		{
			HIVresults << setw(7) << i+1;
			accuracy = HIVaccuracies[i]*100;
			HIVresults << setw(20) << setprecision(4) << accuracy << "%" << endl;
		}
		HIVresults << "+++++++++++++++++++++++++++++++++++++" << endl;
		HIVresults.close();
		cout << "HIV results have been saved to file." << endl; cout << "" << endl;
	}

	int A = 0;

	//system("pause");
	return 0;
}
