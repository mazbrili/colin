#include "wgv_load.h"

wgv_load::wgv_load(const wgv_load &other)
{
    _typ = other.typ();
    _Px = other.Px();
    _Pz = other.Pz();
    _M = other.M();
    _at = other.at();
}
