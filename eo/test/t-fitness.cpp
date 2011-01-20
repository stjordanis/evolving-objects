//-----------------------------------------------------------------------------
// t-eofitness.cpp
// (c) GeNeura Team 1998
//-----------------------------------------------------------------------------

#include <time.h>    // time
#include <stdlib.h>  // srand, rand
#include <iostream>  // std::cout

#include <ScalarFitness.h>

using namespace eo;

using namespace std;

//-----------------------------------------------------------------------------

template <class Fitness>
int test_fitness(Fitness a, Fitness b)
{
    //  srand(time(0));

    //  Fitness a = aval; //static_cast<double>(rand()) / RAND_MAX;
    //  Fitness b = bval; //static_cast<double>(rand()) / RAND_MAX;

    std::cout.precision(2);

    unsigned repeat = 2;
    while (repeat--)
	{
	    std::cout << "------------------------------------------------------" << std::endl;
	    std::cout << "testing <    ";
	    if (a < b)
		std::cout << a << " < " << b << "  is true" << std::endl;
	    else
		std::cout << a << " < " << b << "  is false" <<std::endl;

	    std::cout << "testing >    ";
	    if (a > b)
		std::cout << a << " > " << b << "  is true" << std::endl;
	    else
		std::cout << a << " > " << b << "  is false" <<std::endl;

	    std::cout << "testing ==   ";
	    if (a == b)
		std::cout << a << " == " << b << " is true" << std::endl;
	    else
		std::cout << a << " == " << b << " is false" <<std::endl;

	    std::cout << "testing !=   ";
	    if (a != b)
		std::cout << a << " != " << b << " is true" << std::endl;
	    else
		std::cout << a << " != " << b << " is false" <<std::endl;

	    a = b;
	}
    return 1;
}

int main()
{
    std::cout << "Testing minimizing fitness with 1 and 2" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    MinimizingFitness a = 1;
    MinimizingFitness b = 2;

    test_fitness(a, b);

    std::cout << "Testing minimizing fitness with 2 and 1" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    test_fitness(b, a);

    std::cout << "Testing maximizing fitness with 1 and 2" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    MaximizingFitness a1 = 1;
    MaximizingFitness b1 = 2;

    test_fitness(a1,b1);

    std::cout << "Testing maximizing fitness with 2 and 1" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    test_fitness(b1,a1);

}

//-----------------------------------------------------------------------------
