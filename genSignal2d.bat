@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

C:\home\joshxli\research\strat\FIExt\sim\bin\msvc-14.0\release\FIExt-sim.exe -c etc\sim-SU.cfg -a FIExt2dSim --strat.maxPosAge 2 --strat.nearPeriod 1 --strat.riskFactor=0.004589 --strat.ind2UBPct 20  --strat.tpfMult 1.2  --debug 0 %equity%


REM C:\home\joshxli\prod\strat\FIExt\sim\bin\msvc-14.0\debug\FIExt-sim_d.exe -c etc\sim-SU.cfg -a FIExt2d --strat.riskFactor=0.004589  %equity%
