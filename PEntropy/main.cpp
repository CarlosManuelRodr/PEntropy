#include <iostream>
#include <vector>
#include <numeric>
#include <fstream>
#include <string>
#include "auxiliar.h"
#include "optionparser.h"
using namespace std;

vector<Coord<double>> measure_permutation_entropy(vector<Coord<double>> data, int order, int length)
{
	vector<double> date, svalues;
	for (unsigned i = 0; i < data.size(); i++)
	{
		date.push_back(data[i].GetX());
		svalues.push_back(data[i].GetY());
	}

	// Basado en la rutina de David Hernández Enríquez que se encuentra en script/pentropy.py.
	if (date.size() != svalues.size())
	{
		cout << "Error: La longitudes de las listas no coinciden." << endl;
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

			// Inicio de orderamiento burbuja.
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

			// Fin orderamiento.
			for (int j = 0; j < order; j++)
				permutation[permu[j] - 1] = j + 1;

			p_value = 0;
			perminv = permutation;
			reverse(perminv.begin(), perminv.end());

			// Ciclo para obtener el valor de la permutación.
			for (int j = 0; j < order; j++)
				p_value += permutation[j] * pow(10.0, j);

			// Ciclo para acumular valores de permutationes repetidas.
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
	UNKNOWN, FILEPATH, ORDER, LENGTH, HELP
};

const option::Descriptor usage[] =
{
	{ UNKNOWN, 0, "", "",Arg::None, "INSTRUCCIONES: PEntropy [opciones]\n" },
	{ FILEPATH, 0, "f", "file", Arg::Required, "  -f <arg>, \t--file=<arg>  \tArchivo de entrada." },
	{ ORDER, 0, "o", "order", Arg::Required, "  -o <arg>, \t--order=<arg>  \tOrden de la entropia de permutacion." },
	{ LENGTH, 0, "l", "length", Arg::Required, "  -l <arg>, \t--length=<arg>  \tLongitud de intervalo T de la entropia de permutacion." },
	{ HELP,    0,"", "help", Arg::None,    "  \t--help  \tMuestra instrucciones." },
	{ 0,0,0,0,0,0 }
};

int main(int argc, char* argv[])
{
	string filepath = "";
	int order = 6, length = 1000;

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
			filepath = opt.arg;
			break;
		case ORDER:
			order = aux_string_to_num<int>(opt.arg);
			break;
		case LENGTH:
			length = aux_string_to_num<int>(opt.arg);
			break;
		}

	}

	delete[] options;
	delete[] buffer;

	vector<Coord<double>> data = load_file(filepath);
	if (data.empty())
	{
		cout << "El archivo esta vacio." << endl;
		return 1;
	}
	export_csv<double>(measure_permutation_entropy(data, order, length), "PEntropy.csv");
	return 0;
}