@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

C:\home\joshxli\research\strat\FIExt\sim\bin\msvc-14.0\release\FIExt-sim.exe -c etc\sim-SU.cfg -a R2NEX3dStpBeforeLmtTest --strat.maxPosAge 3 --strat.nearPeriod 1 --strat.riskFactor 0.00369  --strat.tpfMult 2  --strat.ind1UB 20 --strat.ind2UB 20 --debug 0 %equity%

