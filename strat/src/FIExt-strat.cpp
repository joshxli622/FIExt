#include "FIExt-strat.hpp"

#include <cmath>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <typeinfo>
#include <pirate/tr/Trader.hpp>
#include <cassert>

using namespace std;
using boost::gregorian::date;
using boost::gregorian::date_duration;
using boost::gregorian::greg_month;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using pirate::ind::EMACalc;
using pirate::ind::na;
using pirate::ind::naValue;
using pirate::ind::TmPx;
using pirate::tr::data::OHLC;

#include <pirate/tr/Trader.hpp>

Strat::Strat(
    const pirate::ind::period_type aNearPeriod,
    const pirate::ind::period_type aFarPeriod,
    const pirate::ind::period_type aFastEMAPeriod,
    const pirate::ind::period_type aSlowEMAPeriod,
    const pirate::ind::period_type aLongAdxPeriod,
    const pirate::ind::period_type aShortAdxPeriod,
    const pirate::ind::period_type aRangePeriod,
    const pirate::ind::period_type aAllocAtrPeriod,
    const pirate::ind::period_type aPctPeriod,

    const int aMaxPosAge,

    const double aInd1LBPct,
    const double aInd1UBPct,
    const double aInd2LBPct,
    const double aInd2UBPct,
    const double aInd3LBPct,
    const double aInd3UBPct,

    const float aRiskFactor,
    const float aTickMult,
    const float aTpfMult)
    : pirate::tr::bar::BarStrat(),
      intNA(naValue<int>()),
      floatNA(naValue<float>()),
      doubleNA(naValue<double>()),

      hiNearCalc(aNearPeriod),
      hiFarCalc(aFarPeriod),
      loNearCalc(aNearPeriod),
      loFarCalc(aFarPeriod),

      fastEMACalc(EMACalc::Period(static_cast<unsigned int>(aFastEMAPeriod)), 0),
      slowEMACalc(EMACalc::Period(static_cast<unsigned int>(aSlowEMAPeriod)), 0),

      longAdxUpCalc(EMACalc::Period(static_cast<unsigned int>(aLongAdxPeriod)), 0),
      longAdxDownCalc(EMACalc::Period(static_cast<unsigned int>(aLongAdxPeriod)), 0),
      longAdxDiffCalc(EMACalc::Period(static_cast<unsigned int>(aLongAdxPeriod)), 0),
      longAdxAtrCalc(aLongAdxPeriod),

      shortAdxUpCalc(EMACalc::Period(static_cast<unsigned int>(aShortAdxPeriod)), 0),
      shortAdxDownCalc(EMACalc::Period(static_cast<unsigned int>(aShortAdxPeriod)), 0),
      shortAdxDiffCalc(EMACalc::Period(static_cast<unsigned int>(aShortAdxPeriod)), 0),
      shortAdxAtrCalc(aShortAdxPeriod),

      rangeHiCalc(aRangePeriod),
      rangeLoCalc(aRangePeriod),
      rangeAtrCalc(aRangePeriod),

      allocAtrCalc(aAllocAtrPeriod),

      ind1LBCalc(aPctPeriod, aInd1LBPct),
      ind1UBCalc(aPctPeriod, aInd1UBPct),
      ind2LBCalc(aPctPeriod, aInd2LBPct),
      ind2UBCalc(aPctPeriod, aInd2UBPct),
      ind3LBCalc(aPctPeriod, aInd3LBPct),
      ind3UBCalc(aPctPeriod, aInd3UBPct),

      hiNear(),
      loNear(),
      hiFar(),
      loFar(),

      longPosAge(0),
      shortPosAge(0),

      longAdx(doubleNA),
      longAdxAtr(doubleNA),
      shortAdx(doubleNA),
      shortAdxAtr(doubleNA),
      rgHi(),
      rgLo(),
      rgAtr(doubleNA),

      allocAtr(doubleNA),

      farPeriod(aFarPeriod),
      maxPosAge(aMaxPosAge),
      ind1LBPct(aInd1LBPct),
      ind1UBPct(aInd1UBPct),
      ind2LBPct(aInd2LBPct),
      ind2UBPct(aInd2UBPct),
      ind3LBPct(aInd3LBPct),
      ind3UBPct(aInd3UBPct),

      ind1Value(doubleNA),
      ind2Value(doubleNA),
      ind1LBValue(doubleNA),
      ind1UBValue(doubleNA),
      ind2LBValue(doubleNA),
      ind2UBValue(doubleNA),
      ind3LBValue(doubleNA),
      ind3UBValue(doubleNA),

      minTickSz(0),
      //      slpgMult(aSlpgMult),
      riskFactor(aRiskFactor),
      tickMult(aTickMult),
      tpfMult(aTpfMult),
      DELTA(0),

      m_pxs(300),
      m_dtms(300),
      m_vols(300),
      m_prevClose(naValue<float>()),
      m_adjCtID(0),

      m_prevDtm(),
      m_adjDtm(),

      m_prevDt(0),
      m_adjDt(0),
      m_prevTm(0),
      m_adjTm(0)
{
    pxHist0.swap(*(new BarBuff(100)));
    pxHist.swap(*(new BarBuff(300)));
    hhHist.swap(*(new DoubleBuff(3)));
    llHist.swap(*(new DoubleBuff(3)));
}

/* virtual */ void Strat::begin()
{

    minTickSz = minTickSize();
    if (debugLevel > 0)
    {
        logHeader()
            << sep << "TsDt"
            << sep << "spread"
            << sep << "adjOp"
            << sep << "adjHi"
            << sep << "adjLo"
            << sep << "adjCl"
            << sep << "vol"

            << sep << "hiNear"
            << sep << "loNear"
            << sep << "hiFar"
            << sep << "loFar"

            << sep << "longAdx"
            << sep << "longAdxAtr"
            << sep << "shortAdx"
            << sep << "shortAdxAtr"
            << sep << "rgHi"
            << sep << "rgLo"
            << sep << "rgAtr"

            << sep << "atrAlloc"
            << sep << "weight"
            << sep << "equity"
            << sep << "exchRt"
            << sep << "ptValue"
            << sep << "nc"

            << sep << "ind1LB"
            << sep << "ind1UB"
            << sep << "ind2LB"
            << sep << "ind2UB"
            << sep << "ind3LB"
            << sep << "ind3UB"

            << sep << "\n";
    }
}

/* virtual */ void Strat::calc()
{

    // std::cout << "dtm = " << dtm << std::endl;

    // MktSim is still spooling in new ct data before the last time
    // of the previous contract (rollover time).  We will not use
    // these data to do anything and hence we just return.
    if (dtm < m_adjDtm && !m_adjDtm.is_special())
    {
        hiNear = hiNearCalc.calc(dtm, high);
        loNear = loNearCalc.calc(dtm, low);
        hiFar = hiFarCalc.calc(dtm, high);
        loFar = loFarCalc.calc(dtm, low);
        pxHist.push_front(OHLC(open, high, low, close));
        hhHist.push_front(hiFar.px);
        llHist.push_front(loFar.px);
        return;
    }
    // now, we have passed the rollover point and we will adjust the saved
    // old contract data (rather than using current contract data before
    // rollover time!) and use the adjusted data for indicator calculation.
    // So, we need to recalculate all indicators.

    // std::cout << "ctID = " << ctID << ", m_adjCtID = " << m_adjCtID
    //           << ", m_adjDtm = " << to_iso_string(m_adjDtm)
    //           << ", dtm = " << to_iso_string(dtm) << std::endl;

    if (ctID != m_adjCtID && !m_pxs.empty())
    {
        m_pxs.pop_back();
        m_dtms.pop_back();
        m_vols.pop_back();
        if (!m_pxs.empty())
        {
            const float clSpread = close - m_prevClose;
            BarBuff::iterator i = m_pxs.begin();
            BarBuff::iterator e = m_pxs.end();
            DtmsBuff::iterator j = m_dtms.begin();
            IntBuff::iterator k = m_vols.begin();
            for (; i != e; ++i, ++k, ++j)
            {
                i->op += clSpread;
                i->cl += clSpread;
                i->hi += clSpread;
                i->lo += clSpread;
                /*
                std::cout << "... ctID = " << ctID << ", m_adjCtID = " << m_adjCtID
                          << ", dtm = " << to_iso_string(*j) << std::endl;
                */
                doCalc(*j, i->op, i->hi, i->lo, i->cl,
                       *k, clSpread, true);
            }
        }
        m_adjCtID = ctID;
    }

    hiNear = hiNearCalc.calc(dtm, high);
    loNear = loNearCalc.calc(dtm, low);
    hiFar = hiFarCalc.calc(dtm, high);
    loFar = loFarCalc.calc(dtm, low);
    pxHist.push_front(OHLC(open, high, low, close));
    hhHist.push_front(hiFar.px);
    llHist.push_front(loFar.px);

    doCalc(dtm, open, high, low, close, volume, 0, false);

    m_pxs.push_back(OHLC(open, high, low, close));
    m_dtms.push_back(dtm);
    m_vols.push_back(volume);

    m_prevClose = close;
    m_prevDtm = dtm;
    m_prevDt = dt;
    m_prevTm = tm;
}

void Strat::doCalc(const boost::posix_time::ptime dt_tm,
                   const float openPx,
                   const float highPx,
                   const float lowPx,
                   const float closePx,
                   const int vol,
                   const float spread,
                   const bool inAdj)
{
    DELTA = minTickSz * tickMult;

    /*
    hiNear = hiNearCalc.calc(dt_tm, highPx);
    loNear = loNearCalc.calc(dt_tm, lowPx);
    hiFar = hiFarCalc.calc(dt_tm, highPx);
    loFar = loFarCalc.calc(dt_tm, lowPx);
    */

    fastEMA = fastEMACalc.calcSkipNA(closePx);
    slowEMA = slowEMACalc.calcSkipNA(closePx);

    allocAtr = allocAtrCalc.calc(openPx, highPx, lowPx, closePx);
    longAdxAtr = longAdxAtrCalc.calc(openPx, highPx, lowPx, closePx);
    longAdx = doubleNA;
    if (!na(longAdxAtr) && longAdxAtr != 0)
    {
        double upMove = highPx - pxHist0[0].hi;
        double downMove = pxHist0[0].lo - lowPx;
        double pDM = (upMove > downMove && upMove > 0) ? upMove : 0;
        double nDM = (downMove > upMove && downMove > 0) ? downMove : 0;
        double pDI = longAdxUpCalc.calc(pDM / longAdxAtr);
        double nDI = longAdxDownCalc.calc(nDM / longAdxAtr);

        if (pDI + nDI != 0)
        {
            longAdx = longAdxDiffCalc.calc(std::fabs((pDI - nDI) / (pDI + nDI)));
        }
        else
        {
            longAdx = doubleNA;
        }

        /*
        logValues() << "\thighPx" << "\t" << highPx << "\t"
                    << "pxHistHi" << "\t" << pxHist0[0].hi << "\t"
                    << "pxHistLo" << "\t" << pxHist0[0].lo << "\t"
                    << "lowPx" << "\t  " << lowPx << "\t"
                    << "upMove" << "\t" << upMove << "\t"
                    << "downMove" << "\t" << downMove << "\t"
                    << "pDM" << "\t" << pDM << "   \t"
                    << "nDM" << "\t" << nDM << "\t"
                    << "pDI" << "\t" << pDI << "\t"
                    << "nDI" << "\t" << nDI << "\t"
                    << "longAdx" << "\t" << longAdx << "\n";
        */
    }

    shortAdxAtr = shortAdxAtrCalc.calc(openPx, highPx, lowPx, closePx);
    if (!na(shortAdxAtr) && shortAdxAtr != 0)
    {
        double upMove = highPx - pxHist0[0].hi;
        double downMove = pxHist0[0].lo - lowPx;
        double pDM = (upMove > downMove && upMove > 0) ? upMove : 0;
        double nDM = (downMove > upMove && downMove > 0) ? downMove : 0;
        double pDI = shortAdxUpCalc.calc(pDM / shortAdxAtr);
        double nDI = shortAdxDownCalc.calc(nDM / shortAdxAtr);
        if (pDI + nDI != 0)
        {
            shortAdx = shortAdxDiffCalc.calc(std::fabs((pDI - nDI) / (pDI + nDI)));
        }
        else
        {
            shortAdx = doubleNA;
        }
    }
    else
    {
        shortAdx = doubleNA;
    }

    rgHi = rangeHiCalc.calc(dt_tm, highPx);
    rgLo = rangeLoCalc.calc(dt_tm, lowPx);
    rgAtr = rangeAtrCalc.calc(openPx, highPx, lowPx, closePx);

    ind1Value = longAdx;
    ind2Value = (rgHi.px - rgLo.px) / rgAtr;
    ind3Value = shortAdx;

    if (!na(ind1Value))
    {
        ind1LBValue = ind1LBCalc.calc(ind1Value);
        ind1UBValue = ind1UBCalc.calc(ind1Value);
    }
    if (!na(ind2Value))
    {
        ind2LBValue = ind2LBCalc.calc(ind2Value);
        ind2UBValue = ind2UBCalc.calc(ind2Value);
    }
    if (!na(ind3Value))
    {
        ind3LBValue = ind3LBCalc.calc(ind3Value);
        ind3UBValue = ind3UBCalc.calc(ind3Value);
    }

    pxHist0.push_front(OHLC(openPx, highPx, lowPx, closePx));
    /*
    pxHist.push_front(OHLC(openPx, highPx, lowPx, closePx));
    hhHist.push_front(hiFar.px);
    llHist.push_front(loFar.px);
    */

    if (debugLevel > 0)
    {
        logValues()
            << sep << to_iso_string(dt_tm)
            << sep << spread
            << sep << openPx
            << sep << highPx
            << sep << lowPx
            << sep << closePx
            << sep << vol

            << sep << hiNear.px
            << sep << loNear.px
            << sep << hiFar.px
            << sep << loFar.px
            
            << sep << longAdx
            << sep << longAdxAtr

            << sep << shortAdx
            << sep << shortAdxAtr

            << sep << rgHi.px
            << sep << rgLo.px
            << sep << rgAtr

            << sep << allocAtr
            << sep << weight()
            << sep << equity()
            << sep << exchRate
            << sep << pointValue()
            << sep << static_cast<unsigned int>(equity() * 0.005 * weight() / (allocAtr * exchRate * pointValue()))

            << sep << ind1LBValue
            << sep << ind1UBValue
            << sep << ind2LBValue
            << sep << ind2UBValue
            << sep << ind3LBValue
            << sep << ind3UBValue

            << sep << "\n";
    }
}

/* virtual */
void Strat::trade()
{
    double ncRaw = equity() * riskFactor * weight() / (allocAtr * exchRate * pointValue());
    if (pxHist[0].cl > pxHist[1].cl)
        ncRaw *= 2;

    unsigned int nc =
        static_cast<unsigned int>(ncRaw);

    // std::cout << "dtm = " << dtm << std::endl;

    if (nc <= 0)
    {
        nc = 1;
    }

    // bool isSigDate = isSigDate();

    bool longCond = testEnterLongFilter();
    bool shortCond = testEnterShortFilter();
    /*
    bool ind1Cond =
        ((ind1UBPct >= 100 || ind1Value < ind1UBValue) &&
         (ind1LBPct <= 0 || ind1Value >= ind1LBValue));
    bool ind2Cond =
        ((ind2UBPct >= 100 || ind2Value < ind2UBValue) &&
         (ind2LBPct <= 0 || ind2Value >= ind2LBValue));

    if (ind1Cond && ind2Cond) { nc *= 2; }
    */

    if (isLong())
    {
        longPosAge++;
        unsigned long longSize = static_cast<unsigned long>(longPosSize());
        assert(longSize != 0);

        if (longSize != 0 && longPosAge >= maxPosAge - 1)
        { /*
             oco(
                 orderOCO(
                     orderExitLong("SX_STP", static_cast<unsigned int>(longSize),
                                   otSTP(roundToTic(static_cast<float>(loNear.px - DELTA)))),

                     orderExitLong("SX_LMT", static_cast<unsigned int>(longSize),
                                   otLMT(roundToTic(static_cast<float>(close + tpfMult*allocAtr))))
                     ),
                 orderExitLong("SX_MOC", static_cast<unsigned int>(longSize),
                               otMOC()));
          */
            exitLong("SX_MOC", longSize, otMOC());
        }
        /*
        else
        {
            exitLong("SX_LON", longSize, otSTP(roundToTic(static_cast<float>(loNear.px - DELTA))));
        }
        */
    }
    else if (isNthBusDaysFromEOM(farPeriod))
    {
        // buy("LE_HIF", nc, otSTP(roundToTic(static_cast<float>(hiFar.px + DELTA))));
        buy("LE_MOO", nc, otMOO());
    }

    if (isShort())
    {
        assert(false);
        /*
        shortPosAge++;
        unsigned long shortSize = static_cast<unsigned long>(shortPosSize());
        assert(shortSize != 0);

        if (shortSize != 0 && shortPosAge >= maxPosAge - 1)
        {
            oco(
                orderOCO(
                    orderExitShort("LX_STP", static_cast<unsigned int>(shortSize),
                                   otSTP(roundToTic(static_cast<float>(hiNear.px + DELTA)))),

                    orderExitShort("LX_LMT", static_cast<unsigned int>(shortSize),
                                   otLMT(roundToTic(static_cast<float>(close - tpfMult*allocAtr))))
                    ),
                orderExitShort("LX_MOC", static_cast<unsigned int>(shortSize),
                               otMOC()));
        } else {
            exitShort("LX_HIN", static_cast<unsigned int>(shortSize),
                      otSTP(roundToTic(static_cast<float>(hiNear.px + DELTA))));
        }
        */
    }
    else if (shortCond)
    {
        // sell("SE_LOF", nc, otSTP(roundToTic(static_cast<float>(loFar.px - DELTA))));
    }

    /*
    logValues() << sep << "pxHist[0].hi = " << pxHist[0].hi << sep
                << "hhHist[1] = " << hhHist[1] << "\n";
    logValues() << sep << "pxHist[0].lo = " << pxHist[0].lo << sep
                << "llHist[1] = " << llHist[1] << "\n";
    */
}

// return true iff this is n-th weekday counting backward from eod of month (include eom).
//           n = 1:   if today is weekday, return today, otherwise, the first weekday before today
//

bool Strat::isNthBusDaysFromEOM(int n)
{
    date currDt = dtm.date();
    date eom = currDt.end_of_month();

    date prevNBD = eom;
    date_duration dd(1);

    if (eom.day_of_week() == 6)
    {
        // Saturday
        prevNBD = eom - dd;
    }
    else if (eom.day_of_week() == 0)
    {
        // Sunday
        prevNBD = eom - dd - dd;
    }

    while (n > 1)
    {
        prevNBD = prevNBD - dd;
        if (prevNBD.day_of_week() == 0 || prevNBD.day_of_week() == 6)
        {
            // weekends
            continue;
        }
        n--;
    }

    return (currDt == prevNBD);
}

// return true iff this is n-th weekday counting forward from eod of month (not include eom).
//           n = 1:   if today is weekday, return today, otherwise, the first weekday before today
//

bool Strat::isNthBusDaysSinceBOM(int n)
{
    date currDt = dtm.date();
    date bom = date(currDt.year(), currDt.month(), 1);

    date_duration dd(1);
    date nextNBD = bom + dd;

    if (bom.day_of_week() == 6)
    {
        // Saturday
        nextNBD = bom + dd + dd;
    }
    else if (bom.day_of_week() == 0)
    {
        // Sunday
        nextNBD = bom + dd;
    }

    while (n > 1)
    {
        nextNBD = nextNBD + dd;
        if (nextNBD.day_of_week() == 0 || nextNBD.day_of_week() == 6)
        {
            // weekends
            continue;
        }
        n--;
    }

    return (currDt == nextNBD);
}

bool Strat::testEnterLongFilter()
{

    bool ind1Cond =
        ((ind1UBPct >= 100 || ind1Value < ind1UBValue) &&
         (ind1LBPct <= 0 || ind1Value >= ind1LBValue));
    bool ind2Cond =
        ((ind2UBPct >= 100 || ind2Value < ind2UBValue) &&
         (ind2LBPct <= 0 || ind2Value >= ind2LBValue));

    bool fresh = (pxHist.size() >= 2 && hhHist.size() > 2 &&
                  pxHist[0].hi < hhHist[1] + DELTA);

    bool trendDown = fastEMA < slowEMA;
    bool trendUp = fastEMA > slowEMA;

    /*
    std::cout << "testEnterLongFilter: "
              << to_iso_string(dtm) << ": ind1Cond = " << ind1Cond
              << ", fresh = " << fresh << ", pxHist[0].hi = " << pxHist[1].hi
              << ", hhHist[1] + DELTA = " << hhHist[1] + DELTA
              << std::endl;
    */
    bool retValue = ind2Cond;
    return retValue;
}

bool Strat::testEnterShortFilter()
{
    bool ind1Cond =
        ((ind1UBPct >= 100 || ind1Value < ind1UBValue) &&
         (ind1LBPct <= 0 || ind1Value >= ind1LBValue));
    bool ind2Cond =
        ((ind2UBPct >= 100 || ind2Value < ind2UBValue) &&
         (ind2LBPct <= 0 || ind2Value >= ind2LBValue));

    bool trendDown = fastEMA < slowEMA;
    bool trendUp = fastEMA > slowEMA;

    bool fresh = (pxHist.size() >= 2 && llHist.size() > 2 &&
                  pxHist[0].lo > llHist[1] - DELTA);

    /*
    std::cout << "testEnterShortFilter: " <<  to_iso_string(dtm) << ": ind1Cond = " << ind1Cond
              << ", fresh = " << fresh << std::endl;
    */

    bool retValue = ind2Cond;
    return retValue;
}

void Strat::filled(const pirate::tr::to::Trade &aTrade)
{
    if (aTrade.order()->side() == pirate::tr::to::Order::Buy)
    {
        if (aTrade.order()->entryExit() == pirate::tr::to::Order::Entry)
        {
            longPosAge = 0;
            // highestSinceLong = high;
            // lowestSinceLong = low;
        }
        else if (aTrade.order()->entryExit() == pirate::tr::to::Order::Exit)
        {
            shortPosAge = intNA;
            // highestSinceShort = naValue<float>();
            // lowestSinceShort = naValue<float>();
        }
    }
    else if (aTrade.order()->side() == pirate::tr::to::Order::Sell)
    {
        if (aTrade.order()->entryExit() == pirate::tr::to::Order::Entry)
        {
            shortPosAge = 0;
            // highestSinceShort = high;
            // lowestSinceShort = low;
        }
        else if (aTrade.order()->entryExit() == pirate::tr::to::Order::Exit)
        {
            longPosAge = intNA;
            // highestSinceLong = naValue<float>();
            // lowestSinceLong = naValue<float>();
        }
    }
}

/* virtual */
void Strat::rollCleanUp()
{
    m_adjDtm = m_prevDtm;
    m_adjDt = m_prevDt;
    m_adjTm = m_prevTm;

    hiNear =
        loNear =
            hiFar =
                loFar =
                    rgHi =
                        rgLo = TmPx();
    rgAtr =
        fastEMA =
            slowEMA =
                longAdx =
                    longAdxAtr =
                        shortAdx =
                            shortAdxAtr =
                                ind1Value =
                                    ind2Value =
                                        ind3Value =
                                            ind1LBValue =
                                                ind1UBValue =
                                                    ind2LBValue =
                                                        ind2UBValue =
                                                            ind3LBValue =
                                                                ind3UBValue = doubleNA;

    fastEMACalc.cleanup();
    slowEMACalc.cleanup();
    hiNearCalc.cleanup();
    loNearCalc.cleanup();
    hiFarCalc.cleanup();
    loFarCalc.cleanup();

    longAdxUpCalc.cleanup();
    longAdxDownCalc.cleanup();
    longAdxDiffCalc.cleanup();
    longAdxAtrCalc.cleanup();

    shortAdxUpCalc.cleanup();
    shortAdxDownCalc.cleanup();
    shortAdxDiffCalc.cleanup();
    shortAdxAtrCalc.cleanup();

    rangeHiCalc.cleanup();
    rangeLoCalc.cleanup();
    rangeAtrCalc.cleanup();

    allocAtrCalc.cleanup();

    ind1LBCalc.cleanup();
    ind1UBCalc.cleanup();
    ind2LBCalc.cleanup();
    ind2UBCalc.cleanup();
    ind3LBCalc.cleanup();
    ind3UBCalc.cleanup();

    pxHist0.clear();
    pxHist.clear();
    hhHist.clear();
    llHist.clear();
}
