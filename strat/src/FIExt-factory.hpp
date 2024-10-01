#ifndef FIExt__FACTORY_HPP
#define FIExt__FACTORY_HPP
#include <pirate/tr/bar/BarStratFactory.hpp>
#include "FIExt-strat.hpp"
#include <cstddef>


/*! A strategy factory implementation.  It is the factory's responsibility 
  to define and parse custom strategy parameters as well as to create
  strategy instances.  This is usually done by heap allocating a new strategy 
  instance and initializing it with the the extracted strategy parameters.

  A factory's methods are called in the following order:
  - prep() : Called once to prepare/define the strategy parameters.
  - init() : Called once to initialize the strategy factory with the extracted
             parameter values.  These values should probably be stored in 
             member variables.
  - create() : Called once for every market to create and initialize
               a new strategy instance.
 */
struct Factory : pirate::tr::bar::BarStratFactory {

    Factory();

    /*! Defines the strategy specific command line or configuration file arguments.
      For details on how to use boost's program options library please see: 
      http://www.boost.org/doc/html/program_options.html

      \param args  The argument definitions.
    */
    /* virtual */ void prep(boost::program_options::options_description &args);


    /*! Extracts the command line or configuration file argument values.
      If a mandatory argument value is missing throw an exception with a resonable error message.
      The thrown exception is idealy of type boost::program_options::error or a subtype of it.

      For details on how to use boost's program options library please see: 
      http://www.boost.org/doc/html/program_options.html

      \param argVals  The argument values.
      \throw  An exception if an argument value is wrong or missing.
    */
    /* virtual */ void init(const boost::program_options::variables_map &argVals);


    /*! Creates a new strategy instance.
      \param trader  The market specific trader instance.
      \return The pointer strategy instance (one instance for each market).
     */
    /* virtual */ Strat* create(const pirate::tr::Trader &trader);

    /*! Appends the strategies version as well as the version of all its dependencies.
      \param versionsPtr  A pointer to a collection of versions.
    */
    /* virtual */ void version(pirate::ver::Versions *versionsPtr);

private:
    pirate::ind::period_type m_nearPeriod;
    pirate::ind::period_type m_farPeriod;
    pirate::ind::period_type m_fastEMAPeriod;
    pirate::ind::period_type m_slowEMAPeriod;
    pirate::ind::period_type m_longAdxPeriod;
    pirate::ind::period_type m_shortAdxPeriod;
    pirate::ind::period_type m_rangePeriod;
    pirate::ind::period_type m_allocAtrPeriod;
    pirate::ind::period_type m_pctPeriod;
    int m_maxPosAge;
    double m_ind1LBPct;
    double m_ind1UBPct;
    double m_ind2LBPct;
    double m_ind2UBPct;
    double m_ind3LBPct;
    double m_ind3UBPct;
    float m_riskFactor;
    float m_tickMult;
    float m_tpfMult;
};

#endif /* FACTORY_HPP */
