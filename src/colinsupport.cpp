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

#include "colinsupport.h"

ColinSupport::ColinSupport()
{
    b = 0x0;

    f_x=0;
    f_z=0;
    f_phi=0;

    alp=0;

	res=0;
}

ColinSupport::ColinSupport(const bool &x, const bool &z, const bool &phi)
{
	b = 0x0;
	if(x) b |= ColinSupport::X;
	if(z) b |= ColinSupport::Z;
	if(phi) b |= ColinSupport::Phi;

	f_x=0;
	f_z=0;
	f_phi=0;
	alp=0;
	res=0;
}

ColinSupport::ColinSupport(Colin::Actions a)
{
    f_x=0;
    f_z=0;
    f_phi=0;

    alp=0;

	res=0;

    if((a & Colin::drawBearingH) == Colin::drawBearingH)
        b |= ColinSupport::X;
    if((a & Colin::drawBearingV) == Colin::drawBearingV)
        b |= ColinSupport::Z;
    if((a & Colin::drawBearingM) == Colin::drawBearingM)
        b |= ColinSupport::Phi;
}


