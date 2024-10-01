@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

for %%A in (5 8 10 11 12 13 14 15 17 20) do C:\home\joshxli\research\strat\FIExt\sim\bin\msvc-14.0\release\FIExt-sim.exe -c etc\sim-SU.cfg -a R1FIExt2dSim_aum_500k --strat.maxPosAge 2 --strat.nearPeriod 1 --strat.riskFactor=0.004134 --strat.ind1UBPct %%A --strat.ind2UBPct %%A  --strat.tpfMult 2  --debug 0 -e 500000

REM %equity%


REM C:\home\joshxli\prod\strat\FIExt\sim\bin\msvc-14.0\debug\FIExt-sim_d.exe -c etc\sim-SU.cfg -a FIExt2d --strat.riskFactor=0.004589  %equity%