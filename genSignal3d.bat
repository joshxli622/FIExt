@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

C:\home\joshxli\research\strat\FIExt\sim\bin\msvc-14.0\release\FIExt-sim.exe -c etc\sim-SU.cfg -a FIExtMOC3d --strat.riskFactor=0.0037 --strat.maxPosAge 3 --strat.nearPeriod 1 --strat.ind2UBPct 100  --strat.tpfMult 2  --debug 0 %equity%


