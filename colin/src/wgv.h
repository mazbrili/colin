
#ifndef WGV_H
#define WGV_H

#include <QtCore/QThread>
#include "wgv_abstract_tw.h"
#include "wgv_tw.h"
#include "matrix.h"

class wgv : public QThread
{
    Q_OBJECT
public:
    explicit wgv(wgv_tw *parent);
    ~wgv();
    void run();

    wgv_tw *file() const {return tw;}

signals:
    void calcFinished();
public slots:
    void writeBack();
private:
    bool abort;
    const inline wgv_node &node(const int &i)const{return nodes[i];}
    const inline wgv_beam &beam(const int &i)const{return beams[i];}
    const inline wgv_load &load(const int &i)const{return loads[i];}
    const inline wgv_node &leftN(const wgv_beam &b)const{return nodes[b.leftNodeI()];}
    const inline wgv_node &rightN(const wgv_beam &b)const{return nodes[b.rightNodeI()];}

    double d_alpha_l(const int &n) {
        if(node(beam(n).leftNodeI()).hasbearing())
            return beam(n).angle()-node(beam(n).leftNodeI()).angle();
        else
            return beam(n).angle();}

    double d_alpha_r(const int &n) {
        if(node(beam(n).rightNodeI()).hasbearing())
            return beam(n).angle()-node(beam(n).rightNodeI()).angle();
        else
            return beam(n).angle();}


    void u_formal();
    void make_k();

    void make_p();
    void make_s0();

    void writeResultsNodes();
    void writeResultsBeams();

    int f_n;
    int a_n;

    vector<double> s0;
    vector<double> p;
    vector<double> u;
    vector<double> ua;
    vector<double> pa;

    vector<int> u_indexed;
    vector<int> k_index;

    quadmatrix<double> K;

    wgv_tw *tw;
    wgv_node *nodes;
    wgv_beam *beams;
    wgv_load *loads;
    polynom<1> *px;
    polynom<1> *pz;
    double *dT;
    double *dTz;

    double globalMMax;
    double globalPMax;
    double globalUMax;

    int node_n;
    int beam_n;
    int load_n;
};

#endif // WGV_H
