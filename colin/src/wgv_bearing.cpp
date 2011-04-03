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
