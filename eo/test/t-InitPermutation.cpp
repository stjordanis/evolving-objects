//-----------------------------------------------------------------------------
// t-InitPermutation.cpp
//-----------------------------------------------------------------------------

#include <eo>

#include <Int.h>

using namespace eo;

//-----------------------------------------------------------------------------

typedef Int<double> Chrom;

//-----------------------------------------------------------------------------


double real_value(const Chrom & _chrom)
{
  double sum = 0;
  for (unsigned i = 0; i < _chrom.size(); i++)
      sum += _chrom[i];
  return sum/_chrom.size();
}

// Return true if the given chromosome corresponds to a permutation
// There must be an nicer way to do it (set?) ...
bool check_permutation(const Chrom & _chrom)
{
	for (unsigned i = 0; i < _chrom.size(); ++i)
		for (unsigned j = 0; j < _chrom.size(); ++j)
			if(i!=j)
				if(_chrom[i]==_chrom[j]){
						 std::cout << " Error: Wrong permutation !" << std::endl;
						 std::string s;
						 s.append( " Wrong permutation in t-InitPermutation");
	    				 throw std::runtime_error( s );	
				}
	return true;				
}

int main()
{
  const unsigned POP_SIZE = 8, CHROM_SIZE = 16;
  unsigned i;

   // a chromosome randomizer
  InitPermutation <Chrom> random(CHROM_SIZE);
  
   // the population: 
  Pop<Chrom> pop;
	
   // Evaluation
  EvalFuncPtr<Chrom> eval(  real_value );
 
  for (i = 0; i < POP_SIZE; ++i)
    {
      Chrom chrom(CHROM_SIZE);
      std::cout << " Initial chromosome n°" << i << " : " << chrom << "..." <<  std::endl;
      random(chrom);
      eval(chrom);
      std::cout << " ... becomes : " << chrom << " after initialization" << std::endl;
      check_permutation(chrom);
      pop.push_back(chrom);
    }
    
  return 0;
}

//-----------------------------------------------------------------------------
