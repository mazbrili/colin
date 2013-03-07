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

#ifndef POLYNOM_H
#define POLYNOM_H

#include <cmath>
#include <QtCore/QTextStream>


class function
{
public:
	function(){};
	virtual double operator()(const double &x)const=0;
	virtual int maxCount()const=0;
	virtual double max(const int &i)const=0;
};

namespace DiffOp { enum Operator{dx};}

template<int T>class polynom : public function
{
public:
    polynom(){}
    explicit polynom(const double &a0){Q_ASSERT(T==0); a[0]=a0;}
    polynom(const double &a1, const double &a0){Q_ASSERT(T==1); a[0]=a0; a[1]=a1;}
    polynom(const double &a2, const double &a1, const double &a0){Q_ASSERT(T==2); a[0]=a0; a[1]=a1; a[2]=a2;}
    polynom(const double &a3, const double &a2, const double &a1, const double &a0){Q_ASSERT(T==3); a[0]=a0; a[1]=a1; a[2]=a2; a[3]=a3;}
    polynom(const double &a4, const double &a3, const double &a2, const double &a1, const double &a0){Q_ASSERT(T==4); a[0]=a0; a[1]=a1; a[2]=a2; a[3]=a3; a[4]=a4;}

    double operator()(const double &x) const {
        double p = 0;
        double xx = 1;
        for(int i=0; i<T+1; i++){
            p+= a[i]*xx;
            xx*=x;}
        return p;}

    polynom<T-1> operator/(DiffOp::Operator op) const {
        (void)op;
        polynom<T-1> differentiation;
        for(int i=0; i<T; i++)
            differentiation.a[i]=a[i+1]*(i+1);
        return differentiation;}

    polynom<T+1> operator*(DiffOp::Operator op) const {
        (void)op;
        polynom<T+1> integration;
        integration.a[0]=0;
        for(int i=1; i<T+2; i++)
            integration.a[i]=a[i-1]/i;
        return integration;}

    polynom<T> operator+(const polynom &p) const {
        polynom<T> sum;
        for(int i=0; i<T+1; i++)
            sum.a[i]=a[i]+p.a[i];
        return sum;}

    void operator+=(const polynom &p){
        for(int i=0; i<T+1; i++)
            a[i]+=p.a[i];}

    polynom<T> operator-(const polynom<T> &p) const {
        polynom<T> diff;
        for(int i=0; i<T+1; i++)
            diff.a[i]=a[i]-p.a[i];
        return diff;}

    polynom<T> operator+(const double &P) const {
        polynom<T> sum = *this;
        sum.a[0]+=P;
        return sum;}

    polynom<T> operator-(const double &P) const {
        polynom<T> diff = *this;
        diff.a[0]-=P;
        return diff;}

    polynom<T> operator*(const double &P) const {
        polynom<T> multi;
        for(int i=0; i<T+1; i++)
            multi.a[i]=a[i]*P;
        return multi;}

    polynom<T> operator/(const double &P) const {
        polynom<T> div;
        for(int i=0; i<T+1; i++)
            div.a[i]=a[i]/P;
        return div;}

    polynom<2*T> operator*(const polynom<T> &p) const {
        polynom<2*T> multi;
        for(int i=0; i<2*T+1; i++)multi.a[i]=0;
        for(int i=0; i<T+1; i++){
            for(int j=0; j<T+1; j++)
                multi.a[i+j]+=a[i]*p.a[j];}
        return multi;}

    void operator*=(const double &d){
        for(int i=0; i<T+1; i++)
            a[i]*=d;}

    void operator/=(const double &d){
        for(int i=0; i<T+1; i++)
            a[i]/=d;}

	double max(const int &i) const {
        return max_[i];}

	int maxCount() const {
		if(T-2<0)
			return 0;
		return T-1;
	}

    void inline calcMax(){}

	friend QTextStream& operator<<(QTextStream &os, const polynom &p){
		for(int i=T; i>=0; --i)
			os << " + " << p.a[i] << " x^" << i;
		return os;}

	QString toAscii(const double &prefix, const char &f, const int &precison) const
	{
		QString s;
		for(int i=T; i>=0; --i)
		{
			if(a[i]!=0)
			{
				if(a[i]>0 && !s.isEmpty())
					s.append("+");
				s.append(QString::number(a[i]*prefix, f, precison));
				if(i!=0)
				{
					s.append(QString("*x"));
					s.append(QString("^%1").arg(i));
				}
			}
		}
		if(s.isEmpty())
			s.append(QString::number(0, f, precison));
		return s;
	}

	QString toString(const double &prefix, const char &f, const int &precison) const
    {
        QString s;
        for(int i=T; i>=0; --i)
        {
            if(a[i]!=0)
            {
                if(a[i]>0 && !s.isEmpty())
                    s.append("+");
                s.append(QString::number(a[i]*prefix, f, precison));
		if(i!=0)
		{
		    s.append(QString("x"));
		    switch(i)
		    {
		    case 2:
			s.append(QChar(0x00B2));
			break;
		    case 3:
			s.append(QChar(0x00B3));
			break;
		    case 4:
			s.append(QChar(0x2074));
			break;
		    case 5:
			s.append(QChar(0x2075));
			break;
		    case 6:
			s.append(QChar(0x2076));
			break;
		    case 7:
			s.append(QChar(0x2077));
			break;
		    case 8:
			s.append(QChar(0x2078));
			break;
		    case 9:
			s.append(QChar(0x2079));
			break;
		    default:
			break;
		    }
		}
	    }
        }
        if(s.isEmpty())
            s.append(QString::number(0, f, precison));
        return s;
    }

    friend bool operator==(const polynom &one, const polynom &sec){
        for(int i=0; i<T+1; i++)
            if(one.a[i]!=sec.a[i]) return false;
        return true;}

    int order() const {return T;}

    const double &an(int i){return a[i];}

    double a[T+1];
	double max_[T<2?0:T-1];
};

template<> inline void polynom<0>::calcMax(){}
template<> inline void polynom<1>::calcMax(){}
template<> inline void polynom<2>::calcMax(){
	if(a[2]==0)
		max_[0]=0;
	else
		max_[0]=-a[1]/2/a[2];}
template<> inline void polynom<3>::calcMax(){
	if(a[3]==0){
		if(a[2]==0)
			max_[0]=0;
		else
			max_[0]=-a[1]/2/a[2];
		max_[1]=0;
        return;}
    double d = 4*a[2]*a[2]-12*a[3]*a[1];
    if(d<0){
        max_[0]=0;
        max_[1]=0;}
    else if(d==0){
        max_[0]=-2*a[2]/6/a[1];
        max_[1]=0;}
    else{
        max_[0]=(-2*a[2]+sqrt(d))/6/a[3];
        max_[1]=(-2*a[2]-sqrt(d))/6/a[3];}}


#endif // POLYNOM_H
