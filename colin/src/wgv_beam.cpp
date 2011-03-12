#include "wgv_beam.h"

wgv_beam::wgv_beam(wgv_abstract_tw *parent)
{
    par=parent;
    for(int i=0; i<6; i++){gel[i]=0; spr[i]=0;}
    mt = 0;
    qs = 0;
    dirty = true;
}

wgv_beam::wgv_beam(wgv_abstract_tw *parent, int leftN, int rightN, int Mat, int qS)
{
    par=parent;
    for(int i=0; i<6; i++){gel[i]=0; spr[i]=0;}
    node_l=leftN;
    node_r=rightN;
    mt=Mat;
    qs=qS;
    dirty = true;
}

wgv_beam::wgv_beam(wgv_abstract_tw *parent, int leftN, int rightN, QString Mat, QString qS)
{
    par=parent;
    for(int i=0; i<6; i++){gel[i]=0; spr[i]=0;}
    node_l=leftN;
    node_r=rightN;
    mt=LIB.IndexFromMatByName(Mat);
    qs=LIB.IndexFromProfileByName(qS);
    dirty = true;
}

wgv_beam::wgv_beam(const wgv_beam &other)
{
    par=other.par;
    for(int i=0; i<6; i++)
    {
        gel[i]=other.joint(i);
        spr[i]=other.spring(i);
    }
    node_l=other.leftNodeI();
    node_r=other.rightNodeI();
    mt=other.mt;
    qs=other.qs;
    len = other.l();
    alp = other.angle();
    dirty = false;
}

wgv_beam &wgv_beam::operator=(const wgv_beam &other)
{
    par=other.par;
    for(int i=0; i<6; i++)
    {
        gel[i]=other.joint(i);
        spr[i]=other.spring(i);
    }
    node_l=other.leftNodeI();
    node_r=other.rightNodeI();
    mt=other.mt;
    qs=other.qs;
    len = other.l();
    alp = other.angle();
    dirty = false;
    return *this;
}



QTransform wgv_beam::transform() const
{
    QTransform t;
    t.translate(leftNode().x(), leftNode().z());
    t.rotate(angle()*180/M_PI);
    return t;
}


void wgv_beam::calcLen() const
{
    if(leftNode().x()<rightNode().x())
        alp = atan((leftNode().z()-rightNode().z())/(leftNode().x()-rightNode().x()));
    else
        alp = atan((leftNode().z()-rightNode().z())/(leftNode().x()-rightNode().x()))+M_PI;
    if (alp>=M_PI)
        alp= alp - 2*M_PI;

    len = sqrt((leftNode().x()-rightNode().x())*(leftNode().x()-rightNode().x())+
          (leftNode().z()-rightNode().z())*(leftNode().z()-rightNode().z()));
    dirty = false;
}

const double &wgv_beam::angle() const
{
    if(dirty)
        calcLen();

    return(alp);
}

const double &wgv_beam::l() const
{
    if(dirty)
        calcLen();
    return len;
}


void wgv_beam::setJoints(const bool *array)
{
    for(int o=0;o<6; o++)
    {
        gel[o]=array[o];
    }
}

void wgv_beam::setSprings(const double *array)
{
    for(int o=0; o<6; o++)
    {
        spr[o]=array[o];
    }
}


void wgv_beam::setJointsandSprings(const bool *barray, const double *darray)
{
     setJoints(barray);
     setSprings(darray);
}


void wgv_beam::setJoint(const int &pos, const bool &thereIsAJoint)
{
    gel[pos]=thereIsAJoint;
    if(!thereIsAJoint)
        spr[pos]=0;
}

void wgv_beam::setSpring(const int &pos, const double &c_f)
{
    spr[pos]=c_f;
    gel[pos]=true;
}



void wgv_beam::joints(bool *bool_array_6)
{
    for(int o=0; o<6; o++)
        bool_array_6[o]=gel[o];
}

void wgv_beam::springs(double *double_array_6)
{
    for(int o=0; o<6; o++)
        double_array_6[o]=spr[o];
}

double wgv_beam::s(const int& pos) const
{
    switch(pos)
    {
    case 0:
        return _N(0);
    case 1:
        return _Q(0);
    case 2:
        return _M(0);
    case 3:
        return _N(1);
    case 4:
        return _Q(1);
    case 5:
        return _M(1);
    default:
        Q_ASSERT(true);
        return(0);
    }
}

double wgv_beam::v(const int& pos) const
{
    switch(pos)
    {
    case 0:
        return _u(0);
    case 1:
        return _w(0);
    case 2:
        return _p(0);
    case 3:
        return _u(1);
    case 4:
        return _w(1);
    case 5:
        return _p(1);
    default:
        Q_ASSERT(true);
        return(0);
    }
}
