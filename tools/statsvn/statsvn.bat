del logfile.log
svn log -v --xml > logfile.log
java -jar "statsvn.jar" -output-dir ".svn_stats" -include "src/**/*.cpp;src/**/*.h" logfile.log .
del logfile.log
