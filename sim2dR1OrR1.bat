@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

for %%A in (5 10 15 20 25 30 35 40) do C:\home\joshxli\research\strat\FIExt\sim\bin\msvc-14.0\release\FIExt-sim.exe -c etc\sim-SU.cfg -a R1OrFIExt2dSim --strat.maxPosAge 2 --strat.nearPeriod 1 --strat.riskFactor=0.004589 --strat.ind2UBPct %%A --strat.ind1UBPct %%A  --strat.tpfMult 1.2  --debug 0 %equity%


REM C:\home\joshxli\prod\strat\FIExt\sim\bin\msvc-14.0\debug\FIExt-sim_d.exe -c etc\sim-SU.cfg -a FIExt2d --strat.riskFactor=0.004589  %equity%