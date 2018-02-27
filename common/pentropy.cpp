#include <vector>
#include <cmath>
#include <numeric>
#include "pentropy.h"
#include "storage.h"
#include "ThreadPool.h"
using namespace std;


/***********************
 *                     *
 *    Main algorithm   *
 *                     *
 **********************/

// Multi thread.
std::vector<Coord<double>> parallel_function(std::function<Coord<double>(int, vector<double>*, vector<double>*, int, int)> f,
                                             int max_val, vector<double> &date, vector<double> &svalues, int order, int length)
{
    ThreadPool pool(std::thread::hardware_concurrency());
    std::vector<std::future<Coord<double>>> result;
    std::vector<Coord<double>> result_values;
    
    for (int val = 0; val <= max_val; ++val)
        result.push_back(pool.enqueue(f, val, &date, &svalues, order, length));
    
    for (unsigned i = 0; i < result.size(); ++i)
        result_values.push_back(result[i].get());
    
    return result_values;
}

Coord<double> rep_pentropy(int rep, vector<double> *date, vector<double> *svalues, int order, int length)
{
    vector<double> selecf, selecdl, neighbors, permutation, perminv;
    
    int T, swap_num, pass, factorial;
    double H, p_value;
    
    selecf.assign(date->begin() + rep, date->begin() + length + rep);
    selecdl.assign(svalues->begin() + rep, svalues->begin() + length + rep);
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
    return Coord<double>(date->at(rep+T), H);
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
        return vector<Coord<double>>();
    }
    
    vector<Coord<double>> out;
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
        out.push_back(Coord<double>(date[rep+T], H));
    }
    return out;
}
