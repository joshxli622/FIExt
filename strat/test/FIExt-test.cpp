#include "stratest.hpp"

// For more details on how to make use of 
// boost's unit testing framework please see:
// http://www.boost.org/doc/libs/1_38_0/libs/test/doc/html/index.html
BOOST_AUTO_TEST_CASE(testStrategy) {
    const double someValueD = 3.333;
    const float someValueF = 3.333f;
    const int someValue = 3;
    const bool isItReallyTrue = false;
    const bool notItIsNot = true;
    EQD(someValueD, 3.333);
    EQF(someValueF, 3.333f);
    EQ(someValue, 3);
    ISFALSE(isItReallyTrue);
    ISTRUE(notItIsNot);
}

