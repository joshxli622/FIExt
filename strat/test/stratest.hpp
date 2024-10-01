#ifndef STRATEST_HPP
#define STRATEST_HPP

#ifdef _MSC_VER
#  pragma warning( push )
#  pragma warning( disable : 4251 ) // disable missing dll-interface warning
#  pragma warning( disable : 4275 ) // disable non dll-interface as base class warning
#endif

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#ifdef _MSC_VER
#  pragma warning( pop ) 
#endif

#define EQF(L, R) BOOST_CHECK_CLOSE_FRACTION(L, R, 0.01f)
#define EQD(L, R) BOOST_CHECK_CLOSE_FRACTION(L, R, 0.01)
#define EQ(L, R) BOOST_CHECK_EQUAL(L, R)
#define ISTRUE(T) BOOST_CHECK(T)
#define ISFALSE(F) BOOST_CHECK(! (F))
#define MSG(M) BOOST_TEST_MESSAGE(M)
#define PP() BOOST_TEST_PASSPOINT()
#define CP(M) BOOST_TEST_CHECKPOINT(M)

#endif /* STRATEST_HPP */
