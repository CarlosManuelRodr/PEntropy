#include "mathlink.h"
#include "pentropy.h"
#include "ThreadPool.h"
using namespace std;

void permutation_entropy(double* date, int nx1, double* svalues, int nx2, int order, int length)
{
    vector<double> v_date(date, date + nx1);
    vector<double> v_svalues(svalues, svalues + nx2);
    vector<Coord<double>> v_out;
    
    if (nx1 != nx2)
    {
        v_out =  vector<Coord<double>>();
    }
    
    int n = nx1;
    v_out = parallel_function(rep_pentropy, n - length + 1, v_date, v_svalues, order, length);
    
    const int dimensions[2] = {(int)v_out.size(), 2};
    const char* heads[2] = {"List", "List"};
    long depth = 2;
    double *out = new double[2*n];
    
    for (int i=0; i < v_out.size(); i++)
    {
        out[2*i] = v_out[i].GetX();
        out[2*i + 1] = v_out[i].GetY();
    }
    
    MLPutReal64Array(stdlink, out, dimensions, heads, depth);
}

#if defined(WIN32)

int __stdcall WinMain(HINSTANCE hinstCurrent, HINSTANCE hinstPrevious, LPSTR lpszCmdLine, int nCmdShow)
{
    char  buff[512];
    char FAR * buff_start = buff;
    char FAR * argv[32];
    char FAR * FAR * argv_end = argv + 32;
    
    if (!MLInitializeIcon(hinstCurrent, nCmdShow)) return 1;
    MLScanString(argv, &argv_end, &lpszCmdLine, &buff_start);
    return MLMain(argv_end - argv, argv);
}

#else
int main(int argc, char* argv[])
{
    return MLMain(argc, argv);
}

#endif
