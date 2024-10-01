@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

for %%A in (1 2 3 4 5 6) do for %%B in (1 2 3 4 5 6) do C:\home\joshxli\research\strat\FIExt\sim\bin\msvc-14.0\release\FIExt-sim.exe -c etc\sim-SU.cfg -a FIExt --stratFarPeriod %%A --strat.maxPosAge %%B --strat.nearPeriod 1 --strat.riskFactor=0.004589  --strat.tpfMult 1 --debug 0 %equity%


