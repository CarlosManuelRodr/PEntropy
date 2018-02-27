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
