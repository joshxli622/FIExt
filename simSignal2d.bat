@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

C:\home\joshxli\research\strat\FIExt\sim\bin\msvc-14.0\release\FIExt-sim.exe -c etc\sim-SU.cfg -a R2NEX2dStpBeforeLmtTest --strat.maxPosAge 2 --strat.nearPeriod 1 --strat.riskFactor=0.004589  --strat.tpfMult 2 --strat.ind1UBPct 20 --strat.ind2UBPct 20 --debug 0 %equity%
