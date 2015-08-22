/**
* @file auxiliar.h
* @brief Auxiliar functions. Mostly copied from https://github.com/CarlosManuelRodr/FreewayAC.
* @author Carlos Manuel Rodríguez Martínez
* @date 22/08/2015
*/

#pragma once
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include "ThreadPool.h"

/****************************
*                           *
*         Benchmark         *
*                           *
****************************/

template<typename TimeT = std::chrono::milliseconds>
struct aux_measure_time
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
*     Matrix container      *
*                           *
****************************/

template <class T> class Matrix
{
	T** m_matrix;
	unsigned int m_ncols;
	unsigned int m_nrows;
	int m_status;

	void Deallocate();
public:

	Matrix();
	Matrix(unsigned int rows, unsigned int cols);
	~Matrix();

	int SetMatrix(unsigned int rows, unsigned int cols);
	void Assign(T val);

	T*& operator[](const unsigned int pos);
};

template <class T> Matrix<T>::Matrix()
{
	m_ncols = 0;
	m_nrows = 0;
	m_status = 0;
	m_matrix = nullptr;
}
template <class T> Matrix<T>::Matrix(unsigned int rows, unsigned int cols)
{
	m_matrix = nullptr;
	m_ncols = 0;
	m_nrows = 0;
	m_status = this->SetMatrix(rows, cols);
}
template <class T> Matrix<T>::~Matrix()
{
	this->Deallocate();
}
template <class T> void Matrix<T>::Deallocate()
{
	if (m_matrix != nullptr)
	{
		for (unsigned int i = 0; i < m_nrows; i++)
		{
			if (m_matrix[i] != nullptr)
				delete[] m_matrix[i];
		}
		delete[] m_matrix;
	}
}
template <class T> int Matrix<T>::SetMatrix(unsigned int rows, unsigned int cols)
{
	this->Deallocate();
	m_nrows = rows;
	m_ncols = cols;
	try
	{
		m_matrix = new T*[m_nrows];
		for (unsigned int i = 0; i < m_nrows; i++)
			m_matrix[i] = new T[m_ncols];
	}
	catch (std::bad_alloc&)
	{
		return 1;
	}
	return 0;
}
template <class T> void Matrix<T>::Assign(T val)
{
	for (unsigned i = 0; i < m_nrows; i++)
	{
		for (unsigned j = 0; j < m_ncols; j++)
			m_matrix[i][j] = val;
	}
}
template <class T> T*& Matrix<T>::operator[](const unsigned int pos)
{
	return m_matrix[pos];
}

/****************************
*                           *
*       Coordinates         *
*                           *
****************************/

template <class T> class Coord
{
public:
	T m_x, m_y;
	Coord();
	Coord(T x, T y);
	Coord& operator=(const Coord& other);
	bool operator==(const Coord& other);
	bool operator!=(const Coord& other);
	T GetX();
	T GetY();
};

template <class T> Coord<T>::Coord()
{
	m_x = 0;
	m_y = 0;
}
template <class T> Coord<T>::Coord(T x, T y)
{
	m_x = x;
	m_y = y;
}
template <class T> Coord<T>& Coord<T>::operator=(const Coord<T>& other)
{
	m_x = other.m_x;
	m_y = other.m_y;
	return *this;
}
template <class T> bool Coord<T>::operator==(const Coord<T>& other)
{
	if (other.m_x == m_x && other.m_y == m_y)
		return true;
	else
		return false;
}
template <class T> bool Coord<T>::operator!=(const Coord<T>& other)
{
	if (other.m_x == m_x && other.m_y == m_y)
		return false;
	else
		return true;
}
template <class T> T Coord<T>::GetX()
{
	return m_x;
}
template <class T> T Coord<T>::GetY()
{
	return m_y;
}

/****************************
*                           *
*    Auxiliary functions.   *
*                           *
****************************/

template <class T> T aux_string_to_num(const std::string& s)
{
	std::istringstream i(s);
	T x;
	if (!(i >> x))
		return 0;
	return x;
}
template <class T> std::vector<T> aux_csv_to_vector(std::string in)
{
	std::string m_in = in;
	std::vector<T> out;
	int startPos = 0;

	for (unsigned int i = 0; i<m_in.length(); i++)
	{
		if (m_in[i] == ',')
		{
			out.push_back(aux_string_to_num<T>(m_in.substr(startPos, i - startPos)));
			startPos = i + 1;
		}
		if (i + 1 == m_in.length())
		{
			if (m_in[in.length() - 1] == '\n' || m_in[in.length() - 1] == '\r')
				out.push_back(aux_string_to_num<T>(m_in.substr(startPos, m_in.length() - startPos - 1)));
			else
				out.push_back(aux_string_to_num<T>(m_in.substr(startPos, m_in.length() - startPos)));
		}
	}
	return out;
}

/****************************
*                           *
*      Export functions     *
*                           *
****************************/


template <class N> int export_csv(std::vector<N> data_1, std::vector<N> data_2, const std::string filename)
{
	if (data_1.size == 0 || data_2.size == 0)
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