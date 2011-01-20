// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// SigBinaryFlight.h
// (c) OPAC 2007, INRIA Futurs DOLPHIN
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
    		 clive.canape@inria.fr
 */
//-----------------------------------------------------------------------------

#ifndef SIGBINARYFLIGHT_H
#define SIGBINARYFLIGHT_H

//-----------------------------------------------------------------------------
#include <BinaryFlight.h>
//-----------------------------------------------------------------------------

namespace eo
{

    /**
     * 	Binary flight for particle swarm optimization based on the sigmoid function. Velocities are expected to be "double"
     *  Consider Pi to be the i-th position of a particle and Vi to be the i-th velocity of the same particle :
     * 		if rand[0;1] < sig(Vi) (Vi <=> double)
     * 			Pi=1
     * 	    else
     * 			Pi=0
     *
     * 	@ingroup Variators
     */
    template < class POT > class SigBinaryFlight:public BinaryFlight < POT >
    {

    public:

	/**
	 * Constructor.
	 */
	SigBinaryFlight (){slope = 1;}
	SigBinaryFlight (unsigned _slope){slope = _slope;}


	/**
	 * Sigmoid function
	 */
	double sigmoid( double _value)
	{
	    return(1/(1+ exp(- _value/slope)));

	}

	/**
	 * Apply the sigmoid binary flight to a particle.
	 * 
	 */
	void operator  () (POT & _po)
	{

	    for (unsigned j = 0; j < _po.size (); j++)
		{
		    double sigma=rng.uniform(1);

		    if (sigma < sigmoid(_po.velocities[j]))
			_po[j]=1;
		    else
			_po[j]=0;
		}

	    // invalidate the fitness because the positions have changed
	    _po.invalidate();
	}

    private :
	unsigned slope; 
   
    };

}

#endif /*SIGBINARYFLIGHT_H */