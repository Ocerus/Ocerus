cd ..\..\
del logfile.log
svn log -v --xml > logfile.log
java -jar "tools/statsvn/statsvn.jar" -output-dir ".svn_stats" -include "src/**/*.cpp;src/**/*.h" logfile.log "."
del logfile.log
cd tools\statsvn\