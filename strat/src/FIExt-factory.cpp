#include "FIExt-factory.hpp"
using pirate::ind::period_type;

#include "FIExt-strat.hpp"
#include "FIExt-version.hpp"
#include <pirate/ver/Versions.hpp>
#include <pirate/tr/Trader.hpp>
#include <pirate/ind/PirateToolsVersion.hpp>

#include <pirate/tr/cfg/MktDef.hpp>
using pirate::tr::cfg::MktDef;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <pirate/boost/program_options.hpp>
namespace po = boost::program_options;

#include <string>
using std::string;

#include <sstream>
using std::ostringstream;

#include <stdexcept>
using std::runtime_error;

#include <iostream>
using std::cout;
using std::endl;
using pirate::ind::naValue;

Factory::Factory()
    : pirate::tr::bar::BarStratFactory(),
      m_nearPeriod(0),
      m_farPeriod(0),
      m_fastEMAPeriod(0),
      m_slowEMAPeriod(0),
      m_longAdxPeriod(0),
      m_shortAdxPeriod(0),
      m_rangePeriod(0),
      m_allocAtrPeriod(0),
      m_pctPeriod(0),

      m_maxPosAge(naValue<int>()),

      m_ind1LBPct(0.0),
      m_ind1UBPct(100.0),
      m_ind2LBPct(0),
      m_ind2UBPct(100.0),
      m_ind3LBPct(0),
      m_ind3UBPct(100.0),

      m_riskFactor(0.006),
      m_tickMult(0.0),
      m_tpfMult(1.0)

{}

/* virtual */ void Factory::prep(po::options_description &args) {
    po::options_description argsMandCmdLn(" Strategy Mandatory:");
    argsMandCmdLn.add_options()
        ("strat.nearPeriod", 
         po::value<pirate::ind::period_type>(), 
         "Near Period Length for breakout.")
        ("strat.farPeriod", 
         po::value<pirate::ind::period_type>(), 
         "Far Period Length for breakout.")

        ("strat.fastEMAPeriod", 
         po::value<pirate::ind::period_type>(), 
         "Fast EMA Period.")
        ("strat.slowEMAPeriod", 
         po::value<pirate::ind::period_type>(), 
         "Slow EMA Period.")

        ("strat.longAdxPeriod", 
         po::value<pirate::ind::period_type>(), 
         "Period Length for enter long ADX computation.")

        ("strat.shortAdxPeriod", 
         po::value<pirate::ind::period_type>(), 
         "Period Length for enter short ADX computation.")

        ("strat.rangePeriod", 
         po::value<pirate::ind::period_type>(), 
         "Period Length for range computation.")

        ("strat.allocAtrPeriod", 
         po::value<pirate::ind::period_type>(), 
         "Period Length for allocation ATR computation.")
        ("strat.pctPeriod", 
         po::value<pirate::ind::period_type>(), 
         "Rolling Window Length for percentile computation.")

        ("strat.maxPosAge", 
         po::value<int>(),
         "max position age")

        ("strat.ind1LBPct", 
         po::value<double>(),
         "Percentile Lower Bound for ind1")
        ("strat.ind1UBPct", 
         po::value<double>(),
         "Percentile Upper Bound for ind1")
        ("strat.ind2LBPct", 
         po::value<double>(),
         "Percentile Lower Bound for ind2")
        ("strat.ind2UBPct", 
         po::value<double>(),
         "Percentile Upper Bound for ind2")
        ("strat.ind3LBPct", 
         po::value<double>(),
         "Percentile Lower Bound for ind3")
        ("strat.ind3UBPct", 
         po::value<double>(),
         "Percentile Upper Bound for ind3")

        ("strat.riskFactor", 
         po::value<float>(),
         "Risk factor to adjust number of contracts traded.")
        ("strat.tickMult",
         po::value<float>(),
         "Minimal tick multiplier")
        ("strat.tpfMult",
         po::value<float>(),
         "Taking profit multiplier")

        ;
    args.add(argsMandCmdLn);
}

/* virtual */ void Factory::init(const po::variables_map &argVals) {
    if (argVals.count("strat.nearPeriod") == 0) 
        throw po::error("Missing option value: strat.nearPeriod");
    if (argVals.count("strat.farPeriod") == 0)
        throw po::error("Missing option value: strat.farPeriod");

    if (argVals.count("strat.fastEMAPeriod") == 0) 
        throw po::error("Missing option value: strat.fastEMAPeriod");
    if (argVals.count("strat.slowEMAPeriod") == 0)
        throw po::error("Missing option value: strat.slowEMAPeriod");

    if (argVals.count("strat.longAdxPeriod") == 0) 
        throw po::error("Missing option value: strat.longAdxPeriod");
    if (argVals.count("strat.shortAdxPeriod") == 0) 
        throw po::error("Missing option value: strat.shortAdxPeriod");

    if (argVals.count("strat.rangePeriod") == 0)
        throw po::error("Missing option value: strat.rangePeriod");

    if (argVals.count("strat.allocAtrPeriod") == 0) 
        throw po::error("Missing option value: strat.allocAtrPeriod");
    if (argVals.count("strat.pctPeriod") == 0)
        throw po::error("Missing option value: strat.pctPeriod");

    if (argVals.count("strat.maxPosAge") == 0) 
        throw po::error("Missing option value: strat.maxPosAge");

    if (argVals.count("strat.ind1LBPct") == 0) 
        throw po::error("Missing option value: strat.ind1LBPct");
    if (argVals.count("strat.ind1UBPct") == 0)
        throw po::error("Missing option value: strat.ind1UBPct");
    if (argVals.count("strat.ind2LBPct") == 0) 
        throw po::error("Missing option value: strat.ind2LBPct");
    if (argVals.count("strat.ind2UBPct") == 0)
        throw po::error("Missing option value: strat.ind2UBPct");
    if (argVals.count("strat.ind3LBPct") == 0) 
        throw po::error("Missing option value: strat.ind3LBPct");
    if (argVals.count("strat.ind3UBPct") == 0)
        throw po::error("Missing option value: strat.ind3UBPct");


    if (argVals.count("strat.riskFactor") == 0)
        throw po::error("Missing option value: strat.riskFactor");
    if (argVals.count("strat.tickMult") == 0)
        throw po::error("Missing option value: strat.tickMult");
    if (argVals.count("strat.tpfMult") == 0)
        throw po::error("Missing option value: strat.tpfMult");

    m_nearPeriod = argVals["strat.nearPeriod"].as<pirate::ind::period_type>();
    m_farPeriod = argVals["strat.farPeriod"].as<pirate::ind::period_type>();
    m_fastEMAPeriod = argVals["strat.fastEMAPeriod"].as<pirate::ind::period_type>();
    m_slowEMAPeriod = argVals["strat.slowEMAPeriod"].as<pirate::ind::period_type>();
    m_longAdxPeriod = argVals["strat.longAdxPeriod"].as<pirate::ind::period_type>();
    m_shortAdxPeriod = argVals["strat.shortAdxPeriod"].as<pirate::ind::period_type>();
    m_rangePeriod = argVals["strat.rangePeriod"].as<pirate::ind::period_type>();
    m_allocAtrPeriod = argVals["strat.allocAtrPeriod"].as<pirate::ind::period_type>();
    m_pctPeriod = argVals["strat.pctPeriod"].as<pirate::ind::period_type>();

    m_maxPosAge = argVals["strat.maxPosAge"].as<int>();

    m_ind1LBPct = argVals["strat.ind1LBPct"].as<double>();
    m_ind1UBPct = argVals["strat.ind1UBPct"].as<double>();
    m_ind2LBPct = argVals["strat.ind2LBPct"].as<double>();
    m_ind2UBPct = argVals["strat.ind2UBPct"].as<double>();
    m_ind3LBPct = argVals["strat.ind3LBPct"].as<double>();
    m_ind3UBPct = argVals["strat.ind3UBPct"].as<double>();

    m_riskFactor = argVals["strat.riskFactor"].as<float>();
    m_tickMult = argVals["strat.tickMult"].as<float>();
    m_tpfMult = argVals["strat.tpfMult"].as<float>();
}

/* virtual */ Strat* Factory::create(const pirate::tr::Trader &trader) {

    /*
    const MktDef &mktDef = trader.mktDef();
    if (mktDef.argsSize() < 2) {
        ostringstream err;
        err << "Invalid number of market specific arguments in portfolio, mktID: " << trader.mktID();
        throw runtime_error(err.str());
    }
    const float arg0 = lexical_cast<float>(mktDef[0]);
    const string arg1 = lexical_cast<string>(mktDef[1]);
    cout << "Mkt: " << trader.mktID() << ", arg0: " << arg0 << ", arg1: " << arg1 << endl;
    */

    return new Strat(m_nearPeriod, 
                     m_farPeriod,
                     m_fastEMAPeriod,
                     m_slowEMAPeriod,
                     m_longAdxPeriod,
                     m_shortAdxPeriod,
                     m_rangePeriod,
                     m_allocAtrPeriod,
                     m_pctPeriod,

                     m_maxPosAge,

                     m_ind1LBPct,
                     m_ind1UBPct,
                     m_ind2LBPct,
                     m_ind2UBPct,
                     m_ind3LBPct,
                     m_ind3UBPct,
                     m_riskFactor,
                     m_tickMult, 
                     m_tpfMult);
}

/* virtual */ void Factory::version(pirate::ver::Versions *versionsPtr) {
    pirate::ind::version(versionsPtr);
    versionsPtr->append(stratName, stratVersion, __DATE__, __TIME__);
}
