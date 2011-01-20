// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// STLFunctor.h
// (c) Maarten Keijzer 2001
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

    Contact: todos@geneura.ugr.es, http://geneura.ugr.es
             Marc.Schoenauer@polytechnique.fr
             mak@dhi.dk
 */
//-----------------------------------------------------------------------------

#ifndef _STLFunctor_H
#define _STLFunctor_H

#include "Functor.h"

namespace eo
{

    /** @addtogroup Utilities 
     * @{
     */

    /**
       Generic set of classes that wrap an eoF, UF or BF so that they have the
       copy semantics the STL functions usually require (i.e. they can be passed
       by value, rather than the EO standard pass by reference).

       The family consists of STLF, STLUF, STLBF that modify eoF, UF and BF
       respectively
    */
    template <class R>
    class STLF
    {
    public:

	typedef R result_type;

	STLF(F<R>& _f) : f(_f) {}

	R operator()(void)
	{
	    return f();
	}

    private :

	F<R>& f;
    };

#ifdef _MSVC
    /// specialization of void for MSVC users, unfortunately only works for eoF,
    /// as MSVC does not support partial specialization either
    template <>
    void STLF<void>::operator()(void)
    {
	f();
    }
#endif

    /**
       Generic set of classes that wrap an eoF, UF or BF so that they have the
       copy semantics the STL functions usually require (i.e. they can be passed
       by value, rather than the EO standard pass by reference).

       The family consists of STLF, STLUF, STLBF that modify eoF, UF and BF
       respectively
    */
    template <class A1, class R>
    class STLUF : public std::unary_function<A1, R>
    {
    public:
	STLUF(UF<A1,R>& _f) : f(_f) {}

	R operator()(A1 a)
	{
	    return f(a);
	}

    private:
	UF<A1, R>& f;
    };

    /**
       Generic set of classes that wrap an eoF, UF or BF so that they have the
       copy semantics the STL functions usually require (i.e. they can be passed
       by value, rather than the EO standard pass by reference).

       The family consists of STLF, STLUF, STLBF that modify eoF, UF and BF
       respectively
    */
    template <class A1, class A2, class R>
    class STLBF : public std::binary_function<A1, A2, R>
    {
    public:
	STLBF(UF<A1,R>& _f) : f(_f) {}

	R operator()(A1 a1, A2 a2)
	{
	    return f(a1, a2);
	}

    private:

	BF<A1, A2, R>& f;
    };

    //! @todo: put automated wrappers here...

}

/** @} */
#endif

