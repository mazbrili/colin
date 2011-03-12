#include "wgv_bearing.h"

wgv_bearing::wgv_bearing()
{
    b = 0x0;

    f_x=0;
    f_z=0;
    f_phi=0;

    alp=0;
}

wgv_bearing::wgv_bearing(Colin::Actions a)
{
    f_x=0;
    f_z=0;
    f_phi=0;

    alp=0;

    if((a & Colin::drawBearingH) == Colin::drawBearingH)
        b |= wgv_bearing::X;
    if((a & Colin::drawBearingV) == Colin::drawBearingV)
        b |= wgv_bearing::Z;
    if((a & Colin::drawBearingM) == Colin::drawBearingM)
        b |= wgv_bearing::Phi;
}
