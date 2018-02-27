#include <iostream>
#include <vector>
#include <numeric>
#include <fstream>
#include <string>
#include <cmath>
#include <chrono>
#include "pentropy.h"
#include "optionparser.h"
using namespace std;

/*****************************
 *                           *
 *         Benchmark         *
 *                           *
 ****************************/

template<typename TimeT = std::chrono::milliseconds>
struct measure_time
{
    // http://stackoverflow.com/questions/2808398/easily-measure-elapsed-time
    template<typename F, typename ...Args>
    static typename TimeT::rep execution(F func, Args&&... args)
    {
        auto start = std::chrono::system_clock::now();
        func(std::forward<Args>(args)...);
        auto duration = std::chrono::duration_cast< TimeT>
        (std::chrono::system_clock::now() - start);
        return duration.count();
    }
};

/****************************
 *                           *
 *    Auxiliary functions.   *
 *                           *
 ****************************/

template <class T> T string_to_num(const std::string& s)
{
    std::istringstream i(s);
    T x;
    if (!(i >> x))
        return 0;
    return x;
}
template <class T> std::vector<T> csv_to_vector(std::string in)
{
    std::string m_in = in;
    std::vector<T> out;
    int startPos = 0;
    
    for (unsigned int i = 0; i<m_in.length(); i++)
    {
        if (m_in[i] == ',')
        {
            out.push_back(string_to_num<T>(m_in.substr(startPos, i - startPos)));
            startPos = i + 1;
        }
        if (i + 1 == m_in.length())
        {
            if (m_in[in.length() - 1] == '\n' || m_in[in.length() - 1] == '\r')
                out.push_back(string_to_num<T>(m_in.substr(startPos, m_in.length() - startPos - 1)));
            else
                out.push_back(string_to_num<T>(m_in.substr(startPos, m_in.length() - startPos)));
        }
    }
    return out;
}

/*****************************
 *                           *
 *      Export functions     *
 *                           *
 ****************************/


template <class N> int export_csv(std::vector<N> data_1, std::vector<N> data_2, const std::string filename)
{
    if (data_1.size() == 0 || data_2.size() == 0)
    {
        std::cout << "Nothing to export." << std::endl;
        return 1;
    }
    if (data_1.size() != data_2.size())
    {
        std::cout << "Error: Vector sizes don't match." << std::endl;
        return 1;
    }
    
    std::ofstream file(filename.c_str(), std::ofstream::out);
    if (file.is_open())
    {
        for (unsigned i = 0; i < data_1.size(); ++i)
        {
            if (i - data_1.size() != 0)
                file << data_1[i] << ", " << data_2[i] << std::endl;
            else
                file << data_1[i] << ", " << data_2[i];
        }
        file.close();
        return 0;
    }
    else
    {
        std::cout << "Error: Can't create out file." << std::endl;
        return 1;
    }
}

template <class N> int export_csv(std::vector<Coord<N>> data, std::string filename)
{
    std::vector<N> data1, data2;
    for (unsigned i = 0; i < data.size(); ++i)
    {
        data1.push_back(data[i].GetX());
        data2.push_back(data[i].GetY());
    }
    return export_csv<N>(data1, data2, filename);
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
            tmp = csv_to_vector<double>(line);
            out.push_back(Coord<double>(tmp.at(0), tmp.at(1)));
        }
        return out;
    }
    else
    {
        cout << "Error: Couldn't load file." << endl;
        return vector<Coord<double>>();
    }
}


/**********************
*                     *
*   Argument parser   *
*                     *
**********************/

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

        if (msg) PrintError("Option '", option, "' requires an argument.\n");
        return option::ARG_ILLEGAL;
    }
};

enum  OptionIndex {
    UNKNOWN, FILEPATH, OUTNAME, ORDER, LENGTH, SINGLE_THREAD, BENCHMARK, SILENT, HELP
};

const option::Descriptor usage[] =
{
    { UNKNOWN, 0, "", "",Arg::None, "INSTRUCTIONS: PEntropy [options]\n" },
    { FILEPATH, 0, "f", "file", Arg::Required, "  -f <arg>, \t--file=<arg>  \tInput file." },
    { OUTNAME, 0, "s", "outname", Arg::Required, "  -s <arg>, \t--outname=<arg>  \tOutput file name." },
    { ORDER, 0, "o", "order", Arg::Required, "  -o <arg>, \t--order=<arg>  \tPermutation entropy order." },
    { LENGTH, 0, "l", "length", Arg::Required, "  -l <arg>, \t--length=<arg>  \tInterval length." },
    { SINGLE_THREAD, 0, "", "single_thread", Arg::None, " \t--single_thread  \tSingle threaded process." },
    { BENCHMARK, 0, "", "benchmark", Arg::None, " \t--benchmark  \tMeasure execution time." },
    { SILENT, 0, "", "silent", Arg::None, " \t--silent  \tSilent mode." },
    { HELP,    0,"", "help", Arg::None,    "  \t--help  \tShow instructions." },
    { 0,0,0,0,0,0 }
};

int main(int argc, char* argv[])
{
    vector<string> filepaths;
    string outname = "PEntropy";
    int order = 6, length = 1000;
    bool single_thread = false, benchmark = false, silent = false;

    // Execute argument parser.
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
        cout << "Unknown option: " << opt->name << "\n";

    for (int i = 0; i < parse.optionsCount(); ++i)
    {
        option::Option& opt = buffer[i];
        switch (opt.index())
        {
        case FILEPATH:
            filepaths = csv_to_vector<string>(opt.arg);
            break;
        case OUTNAME:
            outname = opt.arg;
            break;
        case ORDER:
            order = string_to_num<int>(opt.arg);
            break;
        case LENGTH:
            length = string_to_num<int>(opt.arg);
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

    for (unsigned int i = 0; i < filepaths.size(); i++)
    {
        cout << "Loading file: " << filepaths[i] << endl;
        vector<Coord<double>> data = load_file(filepaths[i]);
        if (data.empty())
        {
            if (!silent)
                cout << "The file is empty." << endl;
            return 1;
        }

        if (!silent)
        {
            if (benchmark)
                cout << "Benchmarking permutation entropy routines." << endl;
            else
                cout << "Measuring permutation entropy." << endl;
        }

        int r = 0;
        if (benchmark)
        {
            cout << "Single thread: " << measure_time<>::execution(measure_permutation_entropy_st, data, order, length) << " ms " << endl;
            cout << "Multi thread: " << measure_time<>::execution(measure_permutation_entropy_mt, data, order, length) << " ms " << endl;
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
            cout << "Done." << endl;
    }

    return 0;
}
