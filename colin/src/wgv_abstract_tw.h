#ifndef WGV_ABSTRACT_TW_H
#define WGV_ABSTRACT_TW_H

#include <QtCore/QObject>
#include "wgv_node.h"
#include "wgv_lib.h"



class wgv_abstract_tw : public QObject
{
public:
    wgv_abstract_tw(){}

    virtual const wgv_node &node(const int &i) const {return nodes.at(i);}
    virtual int node_n() const {return(nodes.size());}

protected:

    QList<wgv_node> nodes;

};


#endif // WGV_ABSTRACT_TW_H
