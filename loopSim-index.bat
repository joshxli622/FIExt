@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

for %%A in (1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21) do for %%B in (1 2 3 4 5 6) do for %%C in (1 2 3 4 5 6 10) do C:\home\joshxli\research\strat\FIExt\sim\bin\msvc-14.0\release\FIExt-sim.exe -c etc\sim-SU.cfg -a IndexExt-MOO -p etc\pf-rintra-index.tsv --strat.farPeriod %%A --strat.maxPosAge %%B --strat.nearPeriod %%C --strat.riskFactor=0.004589  --strat.tpfMult 1 --debug 0 %equity%


