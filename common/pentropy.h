#ifndef pentropy_h
#define pentropy_h

#include <vector>
#include <functional>
#include "storage.h"

std::vector<Coord<double>> measure_permutation_entropy_mt(std::vector<Coord<double>> data, int order, int length);

std::vector<Coord<double>> measure_permutation_entropy_st(std::vector<Coord<double>> data, int order, int length);

Coord<double> rep_pentropy(int rep, std::vector<double> *date, std::vector<double> *svalues, int order, int length);

std::vector<Coord<double>> parallel_function(std::function<Coord<double>(int, std::vector<double>*, std::vector<double>*, int, int)> f,
                                             int max_val, std::vector<double> &date, std::vector<double> &svalues, int order, int length);

#endif
