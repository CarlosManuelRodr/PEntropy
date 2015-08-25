#include <iostream>
#include <vector>
#include <numeric>
#include <fstream>
#include <string>
#include "auxiliar.h"
#include "optionparser.h"
using namespace std;

// Multi thread.
std::vector<Coord<double>> parallel_function(std::function<Coord<double>(int, vector<double>, vector<double>, int, int)> f, 
	int max_val, vector<double> date, vector<double> svalues, int order, int length)
{
	ThreadPool pool(std::thread::hardware_concurrency());
	std::vector<std::future<Coord<double>>> result;
	std::vector<Coord<double>> result_values;

	for (int val = 0; val <= max_val; val++)
		result.push_back(pool.enqueue(f, val, date, svalues, order, length));

	for (unsigned i = 0; i < result.size(); ++i)
		result_values.push_back(result[i].get());

	return result_values;
}

Coord<double> rep_pentropy(int rep, vector<double> date, vector<double> svalues, int order, int length)
{
	vector<double> entropies, intervals;
	vector<double> selecf, selecdl, neighbors, permutation, perminv;

	int T, swap_num, pass, factorial;
	double H, p_value;

	selecf.assign(date.begin() + rep, date.begin() + length + rep);
	selecdl.assign(svalues.begin() + rep, svalues.begin() + length + rep);
	T = (int)selecf.size() - order + 1;
	factorial = (int)ceil(tgamma(order + 1));
	H = 0;
	Matrix<double> vp(factorial, 3);
	vp.Assign(0.0);

	for (int i = 0; i < T; i++)
	{
		neighbors.assign(selecdl.begin() + i, selecdl.begin() + i + order);
		permutation.assign(order, 0);
		vector<int> permu(order);
		iota(std::begin(permu), std::end(permu), 1);

		// Bubble sort.
		swap_num = 1;
		pass = 1;
		while (pass < (int)neighbors.size() && swap_num == 1)
		{
			swap_num = 0;
			for (int m = 0; m < (int)neighbors.size() - pass; m++)
			{
				if (neighbors[m] > neighbors[m + 1])
				{
					swap(neighbors[m], neighbors[m + 1]);
					swap(permu[m], permu[m + 1]);
					swap_num = 1;
				}
			}
			pass++;
		}

		// End of sort.
		for (int j = 0; j < order; j++)
			permutation[permu[j] - 1] = j + 1;

		p_value = 0;
		perminv = permutation;
		reverse(perminv.begin(), perminv.end());

		// Cycle to obtain permutation value.
		for (int j = 0; j < order; j++)
			p_value += permutation[j] * pow(10.0, j);

		// Cycle to accumulate values of repeated permutations.
		for (int l = 0; l < T; l++)
		{
			if (p_value == vp[l][0])
			{
				vp[l][1]++;
				break;
			}
			else
			{
				if (p_value != vp[l][0] && vp[l][0] == 0)
				{
					vp[l][0] = p_value;
					vp[l][1]++;
					break;
				}
			}
		}
	}

	for (int j = 0; j < factorial; j++)
	{
		if (vp[j][0] == 0)
			continue;
		vp[j][2] = vp[j][1] / (double)T;
		H -= vp[j][2] * log2(vp[j][2]);
	}
	return Coord<double>(date[rep], H);
}

vector<Coord<double>> measure_permutation_entropy_mt(vector<Coord<double>> data, int order, int length)
{
	vector<double> date, svalues;
	for (unsigned i = 0; i < data.size(); i++)
	{
		date.push_back(data[i].GetX());
		svalues.push_back(data[i].GetY());
	}

	if (date.size() != svalues.size())
	{
		cout << "Error: List length don't match." << endl;
		return vector<Coord<double>>();
	}
	int n = (int)date.size();
	return parallel_function(rep_pentropy, n - length + 1, date, svalues, order, length);
}

// Single thread.
vector<Coord<double>> measure_permutation_entropy_st(vector<Coord<double>> data, int order, int length)
{
	vector<double> date, svalues;
	for (unsigned i = 0; i < data.size(); i++)
	{
		date.push_back(data[i].GetX());
		svalues.push_back(data[i].GetY());
	}

	// Based on a routine by David Hernández Enríquez.
	if (date.size() != svalues.size())
	{
		cout << "Error: List length don't match." << endl;
		return vector<Coord<double>>();
	}

	vector<Coord<double>> out;
	vector<double> entropies, intervals;
	vector<double> selecf, selecdl, neighbors, permutation, perminv;

	int T, swap_num, pass, factorial;
	double H, p_value;

	int n = (int)date.size();

	for (int rep = 0; rep < n - length + 1; rep++)
	{
		selecf.assign(date.begin() + rep, date.begin() + length + rep);
		selecdl.assign(svalues.begin() + rep, svalues.begin() + length + rep);
		T = (int)selecf.size() - order + 1;
		factorial = (int)ceil(tgamma(order + 1));
		H = 0;
		Matrix<double> vp(factorial, 3);
		vp.Assign(0.0);

		for (int i = 0; i < T; i++)
		{
			neighbors.assign(selecdl.begin() + i, selecdl.begin() + i + order);
			permutation.assign(order, 0);
			vector<int> permu(order);
			iota(std::begin(permu), std::end(permu), 1);

			// Bubble sort.
			swap_num = 1;
			pass = 1;
			while (pass < (int)neighbors.size() && swap_num == 1)
			{
				swap_num = 0;
				for (int m = 0; m < (int)neighbors.size() - pass; m++)
				{
					if (neighbors[m] > neighbors[m + 1])
					{
						swap(neighbors[m], neighbors[m + 1]);
						swap(permu[m], permu[m + 1]);
						swap_num = 1;
					}
				}
				pass++;
			}

			// End of sort.
			for (int j = 0; j < order; j++)
				permutation[permu[j] - 1] = j + 1;

			p_value = 0;
			perminv = permutation;
			reverse(perminv.begin(), perminv.end());

			// Cycle to obtain permutation value.
			for (int j = 0; j < order; j++)
				p_value += permutation[j] * pow(10.0, j);

			// Cycle to accumulate values of repeated permutations.
			for (int l = 0; l < T; l++)
			{
				if (p_value == vp[l][0])
				{
					vp[l][1]++;
					break;
				}
				else
				{
					if (p_value != vp[l][0] && vp[l][0] == 0)
					{
						vp[l][0] = p_value;
						vp[l][1]++;
						break;
					}
				}
			}
		}

		for (int j = 0; j < factorial; j++)
		{
			if (vp[j][0] == 0)
				continue;
			vp[j][2] = vp[j][1] / (double)T;
			H -= vp[j][2] * log2(vp[j][2]);
		}
		out.push_back(Coord<double>(date[rep], H));
	}
	return out;
}

vector<Coord<double>> load_file(string filepath)
{
	vector<Coord<double>> out;
	vector<double> tmp;

	ifstream file(filepath);
	if (file.is_open())
	{
		string line;
		while (getline(file, line))
		{
			tmp = aux_csv_to_vector<double>(line);
			out.push_back(Coord<double>(tmp.at(0), tmp.at(1)));
		}
		return out;
	}
	else
	{
		cout << "Error al abrir archivo" << endl;
		return vector<Coord<double>>();
	}
}


/****************************
*                           *
*   Parser de argumentos    *
*                           *
****************************/

struct Arg : public option::Arg
{
	static void PrintError(const char* msg1, const option::Option& opt, const char* msg2)
	{
		fprintf(stderr, "%s", msg1);
		fwrite(opt.name, opt.namelen, 1, stderr);
		fprintf(stderr, "%s", msg2);
	}

	static option::ArgStatus Required(const option::Option& option, bool msg)
	{
		if (option.arg != 0)
			return option::ARG_OK;

		if (msg) PrintError("Opcion '", option, "' requiere un argumento.\n");
		return option::ARG_ILLEGAL;
	}
};

enum  OptionIndex {
	UNKNOWN, FILEPATH, OUTNAME, ORDER, LENGTH, SINGLE_THREAD, BENCHMARK, SILENT, HELP
};

const option::Descriptor usage[] =
{
	{ UNKNOWN, 0, "", "",Arg::None, "INSTRUCCIONES: PEntropy [opciones]\n" },
	{ FILEPATH, 0, "f", "file", Arg::Required, "  -f <arg>, \t--file=<arg>  \tArchivo de entrada." },
	{ OUTNAME, 0, "s", "outname", Arg::Required, "  -s <arg>, \t--outname=<arg>  \tNombre del archivo de salida." },
	{ ORDER, 0, "o", "order", Arg::Required, "  -o <arg>, \t--order=<arg>  \tOrden de la entropia de permutacion." },
	{ LENGTH, 0, "l", "length", Arg::Required, "  -l <arg>, \t--length=<arg>  \tLongitud de intervalo T de la entropia de permutacion." },
	{ SINGLE_THREAD, 0, "", "single_thread", Arg::None, " \t--single_thread  \tUna un solo hilo para para realizar el proceso." },
	{ BENCHMARK, 0, "", "benchmark", Arg::None, " \t--benchmark  \tMide tiempo de calculo." },
	{ SILENT, 0, "", "silent", Arg::None, " \t--silent  \tSuprime salida de texto." },
	{ HELP,    0,"", "help", Arg::None,    "  \t--help  \tMuestra instrucciones." },
	{ 0,0,0,0,0,0 }
};

int main(int argc, char* argv[])
{
	vector<string> filepaths;
	string outname = "PEntropy";
	int order = 6, length = 1000;
	bool single_thread = false, benchmark = false, silent = false;

	// Ejecuta parser de argumentos.
	argc -= (argc > 0); argv += (argc > 0);
	option::Stats  stats(usage, argc, argv);
	option::Option *options = new option::Option[stats.options_max];
	option::Option *buffer = new option::Option[stats.buffer_max];
	option::Parser parse(usage, argc, argv, options, buffer);

	if (parse.error())
		return 1;

	if (options[HELP] || argc == 0)
	{
		option::printUsage(cout, usage);
		return 0;
	}

	for (option::Option* opt = options[UNKNOWN]; opt; opt = opt->next())
		cout << "Opcion desconocida: " << opt->name << "\n";

	for (int i = 0; i < parse.optionsCount(); ++i)
	{
		option::Option& opt = buffer[i];
		switch (opt.index())
		{
		case FILEPATH:
			filepaths = aux_csv_to_vector<string>(opt.arg);
			break;
		case OUTNAME:
			outname = opt.arg;
			break;
		case ORDER:
			order = aux_string_to_num<int>(opt.arg);
			break;
		case LENGTH:
			length = aux_string_to_num<int>(opt.arg);
			break;
		case SINGLE_THREAD:
			single_thread = true;
			break;
		case BENCHMARK:
			benchmark = true;
			break;
		case SILENT:
			silent = true;
			break;
		}

	}

	delete[] options;
	delete[] buffer;

	for (int i = 0; i < filepaths.size(); i++)
	{
		cout << "Cargando archivo: " << filepaths[i] << endl;
		vector<Coord<double>> data = load_file(filepaths[i]);
		if (data.empty())
		{
			if (!silent)
				cout << "El archivo esta vacio." << endl;
			return 1;
		}

		if (!silent)
		{
			if (benchmark)
				cout << "Midiendo tiempos calculo de entropia de permutacion." << endl;
			else
				cout << "Midiendo entropia de permutacion." << endl;
		}

		int r = 0;
		if (benchmark)
		{
			cout << "Single thread: " << aux_measure_time<>::execution(measure_permutation_entropy_st, data, order, length) << " ms " << endl;
			cout << "Multi thread: " << aux_measure_time<>::execution(measure_permutation_entropy_mt, data, order, length) << " ms " << endl;
		}
		else
		{
			string outfile;
			if (filepaths.size() == 1)
				outfile = outname + ".csv";
			else
				outfile = outname + to_string(i + 1) + ".csv";

			if (single_thread)
				r = export_csv<double>(measure_permutation_entropy_st(data, order, length), outfile);
			else
				r = export_csv<double>(measure_permutation_entropy_mt(data, order, length), outfile);
		}

		if (r != 0)
			return r;

		if (!silent)
			cout << "Hecho." << endl;
	}

	return 0;
}