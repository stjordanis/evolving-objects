// -*- mode: c++; c-indent-level: 4; c++-member-init-indent: 8; comment-column: 35; -*-

//-----------------------------------------------------------------------------
// eoInit.h
// (c) Maarten Keijzer 2000, GeNeura Team, 2000
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

#ifndef _eoInitVirus_H
#define _eoInitVirus_H

#include <algorithm>

#include <Op.h>
#include <STLFunctor.h>
#include <utils/RndGenerators.h>
#include <Init.h>

/**
    Initializer for binary chromosome with MGE
*/
template <class FitT>
class eoInitVirus: public Init< eoVirus<FitT> > {
public:

  eoInitVirus(unsigned _combien, RndGenerator<bool>& _generator )
	: combien(_combien), generator(_generator) {}
  
  virtual void operator()( eoVirus<FitT>& chrom)
  {
	chrom.resize(combien);
	chrom.virResize(combien);
	std::generate(chrom.begin(), chrom.end(), generator);
	for ( unsigned i = 0; i < combien; i ++ ) {
	  chrom.virusBitSet(i, generator() );
	}
	chrom.invalidate();
  }
  
private :
  unsigned combien;
  /// generic wrapper for eoFunctor (s), to make them have the function-pointer style copy semantics
  STLF<bool> generator;
};

/// Inits the virus with one bit to the left set to one
template <class FitT>
class eoInitVirus1bit: public Init< eoVirus<FitT> > {
public:

  eoInitVirus1bit(unsigned _combien, RndGenerator<bool>& _generator )
	: combien(_combien), generator(_generator) {}
  
  virtual void operator()( eoVirus<FitT>& chrom)
  {
	chrom.resize(combien);
	chrom.virResize(combien);
	std::generate(chrom.begin(), chrom.end(), generator);
	chrom.virusBitSet(0, true );
	chrom.invalidate();
  }
  
private :
  unsigned combien;
  /// generic wrapper for eoFunctor (s), to make them have the function-pointer style copy semantics
  STLF<bool> generator;
};
#endif
