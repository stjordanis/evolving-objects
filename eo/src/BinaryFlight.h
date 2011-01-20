// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// BinaryFlight.h
// (c) OPAC Team, 2007
/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: thomas.legrand@lifl.fr
 */
//-----------------------------------------------------------------------------

#ifndef BINARYFLIGHT_H
#define BINARYFLIGHT_H

//-----------------------------------------------------------------------------
#include <Flight.h>
//-----------------------------------------------------------------------------

namespace eo
{

    /** Abstract class for binary flight of particle swarms. Positions are updated but are expected to be binary.
     *  A function must be used to decide, according to continuous velocities, of the
     *  new positions (0,1 ... ?)
     *
     * @ingroup Core
     */
    template < class POT > class BinaryFlight:public Flight < POT >{};

}

#endif /*BINARYFLIGHT_H */
