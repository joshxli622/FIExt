@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

for %%A in (5 8 10 11 12 13 14 15 17 20) do C:\home\joshxli\research\strat\FIExt\sim\bin\msvc-14.0\release\FIExt-sim.exe -c etc\sim-SU.cfg -a R1FIExt3dSim --strat.riskFactor=0.0037 --strat.maxPosAge 3 --strat.nearPeriod 1 --strat.ind1UBPct %%A --strat.ind2UBPct %%A  --strat.tpfMult 2  --debug 0 -e 400000

REM %equity%


