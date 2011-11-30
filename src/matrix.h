/***********************************************************
 * $Id$
 *
 * Colin
 *
 * Copyright (C) 2011 Matthias Rauter (matthias.rauter@student.uibk.ac.at)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details."
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 *
 * http://www.gnu.org/licenses/.
 *
 * Author: Matthias Rauter
 *
 ***********************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <exception>


class SingularMatrixException : public std::exception
{
    virtual const char* what() const throw()
    {
      return "My exception happened";
    }
};

template <class T>
class quadvector;

template <class T>
class quadmatrix
{
protected:
    T** array;

    int rowcount;
    int cutedAt_;


    void init__()
    {
		array = new T*[rowcount];
        for(int i=0; i<rowcount; i++)
        {
			array[i] = new T[rowcount];
            for(int j=0; j<rowcount; j++)
                array[i][j] = 0;
        }
		cutedAt_=rowcount;
    }

    void delete__()
    {
        for(int i=0; i<rowcount; i++)
            delete[] array[i];
        delete[] array;
    }


public:
    explicit quadmatrix(int n=0)
    {
        rowcount = n;
        init__();
    }

    ~quadmatrix()
    {
        delete__();
    }

    T* operator[] (int i)
    {
        return array[i];
    }

    int fullSize() const
    {
        return rowcount;
    }

    int size() const
    {
        return cutedAt_;
    }

    void setSize(int n)
    {
        delete__();
        rowcount = n;
        init__();
    }

    void cutAt(int before)
    {
        cutedAt_ = before;
    }

    void swap(int a, int b)
    {
        T* t(array[a]);
        array[a] = array[b];
        array[b] = t;
    }

    void mirror()
    {
        for(int i=0; i<fullSize(); i++)
        {
            for(int j=i+1; j<fullSize(); j++)
                array[j][i]  = array[i][j];
        }
    }

	friend QTextStream& operator<< (QTextStream &os, quadmatrix &a)
	{
		for(int i=0; i<a.size(); i++)
		{
			for(int j=0; j<a.size(); j++)
				os << a[i][j] << ";";
			os << endl;
		}
		return os;
	}

    T** getArray() const {return array;}

private:
      quadmatrix(const quadmatrix &);
      quadmatrix &operator=(const quadmatrix &);


};


template <class T>
class submatrix
{
public:
	submatrix(){parent=0;}

    submatrix(quadmatrix<T> &a, const int &r_, const int &c_)
    {
        r = r_;
        c = c_;
        parent = &a;
    }

    T* operator[] (int i)
    {
        return parent->getArray()[(r == 1 ? parent->size() : 0) + i] + (c == 1 ? parent->size() : 0);
    }


    int rowSize() const {
        if(r == 1) return parent->fullSize()-parent->size();
        else return parent->size();}
    int columnSize() const {
        if(c == 1) return parent->fullSize()-parent->size();
        else return parent->size();}


private:
    quadmatrix<T> *parent;
    int r;
    int c;
};


template <class T>
class quadvector
{
public:
    quadvector()
    {
        rowcount = 0;
        init__();
    }

    explicit quadvector(int n)
    {
        rowcount = n;
        init__();
    }

    quadvector(quadvector &o)
    {
        rowcount = o.rowcount;
		if(rowcount)	array = new T[rowcount];
		else			array = NULL;
        for(int i=0; i<rowcount; i++)
            array[i]=o[i];
    }

    quadvector &operator=(const quadvector &o)
    {
		if(array) delete[] array;
        rowcount = o.rowcount;
		if(rowcount)	array = new T[rowcount];
		else			array = NULL;
        for(int i=0; i< rowcount; i++)
            array[i]=o.array[i];
        return *this;
    }

    ~quadvector()
    {
		if(array) delete[] array;
    }

    T& operator[] (int i)
    {
        return array[i];
    }

    quadvector<T> operator+(quadvector &v)
    {
        quadvector<T> sum((size()>v.size()) ? size() : v.size());
        for(int i=0; i<size(); i++)
            sum[i]=array[i];
        for(int i=0; i<v.size(); i++)
            sum[i]+=v[i];
        return sum;
    }

    quadvector<T> operator/(quadmatrix<T> &a)
    {
		T fak;

		quadvector<T> temp(a.size());

        for(int n=0; n<a.size()-1; n++)
        {
            for(int m=n+1; m<a.size(); m++)
            {
                if(fabs(a[m][n]) > fabs(a[n][n]))
                {
                    a.swap(m, n);
                    swap(m, n);
                }
                if(m==a.size()-1 && fabs(a[n][n])<1e-05)
                    throw SingularMatrixException();
            }
            for(int m=n+1; m<a.size(); m++)
            {
                fak=a[m][n]/a[n][n];
                for(int o=0; o<a.size() && fak!=0; o++)
                    a[m][o]-=fak*a[n][o];
                (*this)[m]-=fak*(*this)[n];
            }
		}
        for(int n=0; n<a.size(); n++)
            temp[n]=0;
        for(int n=a.size()-1; n>-1; n--)
        {
            if(fabs(a[n][n])<1e-05)
            {
                throw SingularMatrixException();
            }
            fak=0;
            for(int m=a.size()-1; m>n; m--)
                fak+=temp[m]*a[n][m];
            temp[n]=((*this)[n]-fak)/a[n][n];
        }
        return temp;
    }

    void operator*=(quadmatrix<T> &a)
    {
        T *temp;
        temp = new T[a.size()];
        for(int i=0; i<a.size(); i++)
            temp[i]=0;
        for(int i=0; i<a.size(); i++)
        {
            for(int j=0; j<a.size(); j++)
                temp[i]+=array[j]*a[i][j];
        }
        delete[] array;
        array = temp;
    }

    quadvector<T> operator*(quadmatrix<T> &a)
    {
        quadvector<T> temp(a.size());
        for(int i=0; i<a.size(); i++)
            temp[i]=0;
        for(int i=0; i<a.size(); i++)
        {
            for(int j=0; j<a.size(); j++)
                temp[i]+=array[j]*a[i][j];
        }
        return temp;
    }

    quadvector<T> operator*(submatrix<T> &a)
    {
        Q_ASSERT(a.columnSize() == size());
        quadvector<T> temp(a.rowSize());
        for(int j=0; j<a.rowSize(); j++)
        {
            for(int i=0; i<a.columnSize(); i++)
            {
                temp[j]+=a[j][i]*(*this)[i];
            }
        }
        return temp;
    }

    int size() const
    {
        return rowcount;
    }

    void setSize(int n)
    {
        delete[] array;
        rowcount = n;
        init__();
    }

    void swap(int a, int b)
    {
        T t(array[a]);
        array[a] = array[b];
        array[b] = t;
    }

	friend QTextStream& operator<< (QTextStream &os, quadvector &v)
	{
		for(int i=0; i<v.size(); i++)
			os << v[i] << ";";
		return os;
	}

private:
    T* array;

    int rowcount;

    void init__()
    {
        array = new T[rowcount];
        for(int i=0; i<rowcount; i++)
            array[i]=0;
    }
};


template<class T>
class quadvectors
{
public:
	quadvectors(int s)
	{
		size=s;
		array = new quadvector<T>*[s];
	}

	void set(int i, quadvector<T>* v)
	{
		array[i]=v;
	}

	~quadvectors()
	{
		delete[] array;
	}

	quadvector<T> &operator[] (int i)
	{
		return *(array[i]);
	}

	static void gauss(quadmatrix<T> &a, quadvectors<T> &x, quadvectors<T> &b)
	{

		for(int i=0; i<x.size; i++){
			x[i].setSize(a.size());
			for(int j=0; j<x[i].size(); j++)
				x[i][j]=0;
		}
#ifndef QT_NO_DEBUG
		QTextStream out(stdout);
		out << "a =" << endl;
		out << a;
		for(int i=0; i<b.size; i++)
		{
			out << "b[" << i << "] =" << endl;
			out << b[i] << endl;
		}
#endif
		T fak;
		for(int v=0; v<b.size; v++)
		{
			for(int j=0; j<a.size(); j++)
				b[v][j]=0;
		}

		for(int n=0; n<a.size()-1; n++)
		{
			for(int m=n+1; m<a.size(); m++)
			{
				if(fabs(a[m][n]) > fabs(a[n][n]))
				{
					a.swap(m, n);
					for(int v=0; v<b.size; v++) b[v].swap(m, n);  //all vectors
				}
				if(m==a.size()-1 && fabs(a[n][n])<1e-05)
					throw SingularMatrixException();
			}
			for(int m=n+1; m<a.size(); m++)
			{
				fak=a[m][n]/a[n][n];
				for(int o=0; o<a.size() && fak!=0; o++)
					a[m][o]-=fak*a[n][o];
				for(int v=0; v<b.size; v++)b[v][m]-=fak*b[v][n]; //all vectors
			}
#ifndef QT_NO_DEBUG
			out << "a[step " << n << "] =" << endl;
			out << a;
			out << "b[1 step " << n << "] =" << endl;
			out << b[1] << endl;
#endif
		}
		for(int n=a.size()-1; n>-1; n--)
		{
			if(fabs(a[n][n])<1e-05)
			{
				throw SingularMatrixException();
			}
			for(int v=0; v<b.size; v++){
				fak = 0;
				for(int m=a.size()-1; m>n; m--)
					fak+=x[v][m]*a[n][m];
				x[v][n]=(b[v][n]-fak)/a[n][n];
#ifndef QT_NO_DEBUG
				out << "x[" << v << "][" << n << "] to " << (b[v][n]-fak)/a[n][n] << endl;
#endif
			}
		}
#ifndef QT_NO_DEBUG
		for(int i=0; i<x.size; i++)
		{
			out << "x[" << i << "] =" << endl;
			out << x[i] << endl;
		}
		out << "a =" << endl;
		out << a;
		for(int i=0; i<b.size; i++)
		{
			out << "b[" << i << "] =" << endl;
			out << b[i] << endl;
		}
#endif
	}

private:
	int size;
	quadvector<T>** array;
};

typedef quadvector<double> lfvector;
typedef quadvector<int> devector;


typedef quadmatrix<double> lfquadmatrix;
typedef quadmatrix<int> dquadmatrix;

#endif // MATRIX_H
