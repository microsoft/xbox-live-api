run command to generate the header:
mc -um ERA_ETW.man -z era_etw

start tracing for ERA:
Start tracing with system logs:
xbrun /x/title /O tracelog -start CustomSession -f d:\custom03.etl -eflag PROC_THREAD+LOADER+DPC+INTERRUPT+CSWITCH+PROFILE -guid #{9594A560-E985-4EE6-B0B5-0DAC4F924144} -stackwalk PROFILE+CSWITCH

Start tracing with XSAPI etw only:
xbrun /x/title /O tracelog -start CustomSession -f d:\custom03.etl -guid #{9594A560-E985-4EE6-B0B5-0DAC4F924144}

Stop tracign for ERA:
xbrun /x/title /O tracelog -stop CustomSession

Merge if you started with system:
xbrun /x/title /O tracelog -merge d:\custom03.etl d:\custom03_merge.etl

copy to your local machine:
If you merged with system log: xbcp /x/title xd:\custom03_merge.etl f:
If you didn't merge: xbcp /x/title xd:\custom03.etl f:


