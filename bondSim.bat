@echo off

set equity=
if "%1"=="" (
   set equity="" 
) else (  
  set equity= -e c%1
)

for %%A in (4)  do (
for %%B in (3) do (
C:\home\joshxli\prod\strat\FIExt\sim\bin\msvc-14.0\release\FIExt-sim.exe ^
-c etc\prod-BBAT.cfg ^
-a BondExt ^
--strat.farPeriod %%A ^
--strat.maxPosAge %%B ^
--strat.nearPeriod 10 ^
--strat.riskFactor=0.004589  ^
--strat.tpfMult 1 ^
--debug 1 ^
%equity%
))


