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
#include <exception>


class SingularMatrixException : public std::exception
{
    virtual const char* what() const throw()
    {
      return "My exception happened";
    }
};


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
class vector
{
public:
    vector()
    {
        rowcount = 0;
        init__();
    }

    explicit vector(int n)
    {
        rowcount = n;
        init__();
    }

    vector(vector &o)
    {
        rowcount = o.rowcount;
        array = new T[rowcount];
        for(int i=0; i<rowcount; i++)
            array[i]=o[i];
    }

    vector &operator=(const vector &o)
    {
        delete[] array;
        rowcount = o.rowcount;
        array = new T[rowcount];
        for(int i=0; i< rowcount; i++)
            array[i]=o.array[i];
        return *this;
    }

    ~vector()
    {
        delete[] array;
    }

    T& operator[] (int i)
    {
        return array[i];
    }

    vector<T> operator+(vector &v)
    {
        vector<T> sum((size()>v.size()) ? size() : v.size());
        for(int i=0; i<size(); i++)
            sum[i]=array[i];
        for(int i=0; i<v.size(); i++)
            sum[i]+=v[i];
        return sum;
    }

    vector<T> operator/(quadmatrix<T> &a)
    {
        T fak;
        vector<T> temp(0);

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
        temp = vector<T>(a.size());
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

    vector<T> operator*(quadmatrix<T> &a)
    {
        vector<T> temp(a.size());
        for(int i=0; i<a.size(); i++)
            temp[i]=0;
        for(int i=0; i<a.size(); i++)
        {
            for(int j=0; j<a.size(); j++)
                temp[i]+=array[j]*a[i][j];
        }
        return temp;
    }

    vector<T> operator*(submatrix<T> &a)
    {
        Q_ASSERT(a.columnSize() == size());
        vector<T> temp(a.rowSize());
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

    friend QTextStream& operator<< (QTextStream &os, vector &v)
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


typedef vector<double> lfvector;
typedef vector<int> devector;


typedef quadmatrix<double> lfquadmatrix;
typedef quadmatrix<int> dquadmatrix;

#endif // MATRIX_H
