#ifndef FIExt__STRAT_HPP
#define FIExt__STRAT_HPP

#include <pirate/tr/bar/BarStrat.hpp>

#include <pirate/ind/Indicator.hpp>
#include <pirate/ind/TmPx.hpp>
#include <pirate/ind/HighTm.hpp>
#include <pirate/ind/LowTm.hpp>
#include <pirate/ind/Var.hpp>
#include <pirate/ind/EMA.hpp>
#include <pirate/ind/SMA.hpp>
#include <pirate/ind/RSI.hpp>
#include <pirate/ind/ATR.hpp>
#include <pirate/ind/Pctile.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <pirate/tr/data/OHLC.hpp>
#include <pirate/data/CyclicBuffer.hpp>
#include <pirate/tr/Position.hpp>
#include <pirate/tr/OMS.hpp>
#include <pirate/tr/Orders.hpp>
#include <pirate/tr/to/Order.hpp>
#include <pirate/tr/to/OrderOCO.hpp>
#include <pirate/tr/to/OrderOTO.hpp>
#include <pirate/tr/to/Trade.hpp>
#include <pirate/tr/Trader.hpp>
#include <pirate/tr/to/LMT.hpp>
#include <pirate/tr/to/LMTT.hpp>
#include <pirate/tr/to/STP.hpp>
#include <pirate/tr/to/STPT.hpp>
#include <pirate/tr/to/TifGTB.hpp>
#include <vector>
#include <climits>
#include <cstddef>
#include <string>


/*! Sample high/low breakout strategy implementation.
  For more default on PIRATE's pirate::tr::bar::Strat
  interface please consult its header or API doc.
 */
struct Strat : pirate::tr::bar::BarStrat {
    Strat(const pirate::ind::period_type aNearPeriod,
          const pirate::ind::period_type aFarPeriod,
          const pirate::ind::period_type aFastEMAPeriod,
          const pirate::ind::period_type aSlowEMAPeriod,

          const pirate::ind::period_type aLongAdxPeriod,
          const pirate::ind::period_type aShortAdxPeriod,
          const pirate::ind::period_type aRangePeriod,
          const pirate::ind::period_type aAtrAllocPeriod,    
          const pirate::ind::period_type aPctPeriod, 

          const int maxPosAge,
          
          const double aInd1LBPct,
          const double aInd1UBPct,
          const double aInd2LBPct,
          const double aInd2UBPct,
          const double aInd3LBPct,
          const double aInd3UBPct,

          const float aRiskFactor, 
          const float aTickMult,
          const float aTpfMult
);
    
    /* virtual */ 
    void calc();
    void doCalc(const boost::posix_time::ptime dt_tm,
                const float openPx,
                const float highPx,
                const float lowPx,
                const float closePx,
                const int vol, 
                const float spread,
                const bool logOn);

    /* virtual */
    void begin();
    /* virtual */ 
    void trade();
    /* virtual */ 
    void rollCleanUp();
    /* virtual */ void filled(const pirate::tr::to::Trade &aTrade);
    bool isNthBusDaysFromEOM(int n);
    bool isNthBusDaysSinceBOM(int n);
    bool testEnterLongFilter();
    bool testEnterShortFilter();

float roundToTic(const float aPx) const 
{ 
return static_cast<float>(minTickSz * std::floor(aPx / minTickSz + 0.5)); 
} 
    
float roundToTicUp(const float aPx) const
{
return static_cast<float>(minTickSz * std::ceil(aPx / minTickSz)); 
}

    float roundToTicDown(const float aPx) const 
    {
        return static_cast<float>(minTickSz * std::floor(aPx / minTickSz));
    }

private:
    static const int OPEN = 0;
    static const int HIGH = 1;
    static const int LOW = 2;
    static const int CLOSE = 3;

    typedef pirate::data::CyclicBuffer<pirate::tr::data::OHLC> BarBuff;
    typedef pirate::data::CyclicBuffer<double> DoubleBuff; 
    typedef pirate::data::CyclicBuffer<float> FloatBuff;
    typedef pirate::data::CyclicBuffer<pirate::ind::TmPx> TmPxBuff;
    typedef pirate::data::CyclicBuffer<int> IntBuff;
    typedef pirate::data::CyclicBuffer<boost::posix_time::ptime> DtmsBuff;

    int intNA;
    float floatNA;
    double doubleNA;

    pirate::ind::EMACalc fastEMACalc;
    pirate::ind::EMACalc slowEMACalc;

    pirate::ind::HighTmCalc hiNearCalc;
    pirate::ind::HighTmCalc hiFarCalc;
    pirate::ind::LowTmCalc loNearCalc;
    pirate::ind::LowTmCalc loFarCalc;

    pirate::ind::EMACalc longAdxUpCalc; // UpMove
    pirate::ind::EMACalc longAdxDownCalc; // DownMove
    pirate::ind::EMACalc longAdxDiffCalc; // difference of above two   
    pirate::ind::ATRCalc longAdxAtrCalc;

    pirate::ind::EMACalc shortAdxUpCalc; //     UpMove
    pirate::ind::EMACalc shortAdxDownCalc; // DownMove
    pirate::ind::EMACalc shortAdxDiffCalc; // difference of above two   
    pirate::ind::ATRCalc shortAdxAtrCalc;

    pirate::ind::HighTmCalc rangeHiCalc;
    pirate::ind::LowTmCalc rangeLoCalc;
    pirate::ind::ATRCalc rangeAtrCalc;

    pirate::ind::ATRCalc allocAtrCalc;

    pirate::ind::PctileCalc ind1LBCalc;
    pirate::ind::PctileCalc ind1UBCalc;
    pirate::ind::PctileCalc ind2LBCalc;
    pirate::ind::PctileCalc ind2UBCalc;
    pirate::ind::PctileCalc ind3LBCalc;
    pirate::ind::PctileCalc ind3UBCalc;

    double fastEMA;
    double slowEMA;

    pirate::ind::TmPx hiNear;
    pirate::ind::TmPx loNear;
    pirate::ind::TmPx hiFar;
    pirate::ind::TmPx loFar;

    int longPosAge;
    int shortPosAge;

    double longAdx;
    double longAdxAtr;
    double shortAdx;
    double shortAdxAtr;

    pirate::ind::TmPx rgHi;
    pirate::ind::TmPx rgLo;
    double rgAtr;
    double allocAtr;

    int farPeriod;
    int maxPosAge;

    double ind1LBPct;
    double ind1UBPct;
    double ind2LBPct;
    double ind2UBPct;
    double ind3LBPct;
    double ind3UBPct;
    double ind1Value;
    double ind2Value;
    double ind3Value;
    double ind1LBValue;
    double ind1UBValue;
    double ind2LBValue;
    double ind2UBValue;
    double ind3LBValue;
    double ind3UBValue;

    BarBuff pxHist;   // history of unajusted contracts
    BarBuff pxHist0;  // history of ajusted front contracts only 
    DoubleBuff hhHist;
    DoubleBuff llHist;

    float minTickSz;
    float riskFactor;
    float tickMult;
    float tpfMult;

    float DELTA;
    
  //! CyBuf of closing prices
    BarBuff m_pxs;
    DtmsBuff m_dtms;
    IntBuff m_vols;
    //! Old front contract's prior closing price, used for spread calc.
    float m_prevClose;
    //! The last contract ID that we spre   ad adjusted.
    pirate::tr::ct_id_type m_adjCtID;

#ifdef _MSC_VER
#  pragma warning( push )
#    pragma warning( disable : 4251 ) // disable missing dll-interface warning
#endif
    //! The previous business day's date-time.
    boost::posix_time::ptime m_prevDtm;

    //! The adjustment date-time (which is one business day prior roll date).
    boost::posix_time::ptime m_adjDtm;
#ifdef _MSC_VER
#  pragma warning( pop ) 
#endif

    //! The previous date (just to make it more debug friendly).
    pirate::tr::bar::dt_type m_prevDt;

    //! The adjustment date (just to make it more debug friendly).
    pirate::tr::bar::dt_type m_adjDt;

    //! The previous time (just to make it more debug friendly).
    pirate::tr::bar::tm_type m_prevTm;

    //! The adjustment time (just to make it more debug friendly).
    pirate::tr::bar::tm_type m_adjTm;


};

#endif /* STRAT_HPP */
