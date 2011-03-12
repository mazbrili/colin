#include "wgv_node.h"




wgv_node::wgv_node()
{
    X=0;
    Z=0;

    bear = NULL;
}

wgv_node::wgv_node(double _x, double _z)
{
    X=_x;
    Z=_z;

    bear = NULL;
}


wgv_node::wgv_node(const wgv_node &n)
{
    X = n.x();
    Z = n.z();
    if(n.hasbearing())
    {
        bear = new wgv_bearing();
        *bear = n.bearing();
    }
    else
    {
        bear = NULL;
    }
}

wgv_node &wgv_node::operator=(const wgv_node &other)
{
    X = other.x();
    Z = other.z();
    if(other.hasbearing())
    {
        if(!hasbearing())
            bear = new wgv_bearing();
        *bear = other.bearing();
    }
    else if(hasbearing())
    {
        delete bear;
        bear = NULL;
    }
    return *this;

}

wgv_node::~wgv_node()
{
    if(bear != NULL)
        delete bear;
}

bool wgv_node::hasbearing() const
{
    if(bear == NULL)
        return false;
    else
        return true;
}

void wgv_node::setBearing(wgv_bearing bn)
{
    if(bear == NULL)
        bear = new wgv_bearing();
    *bear = bn;
}


