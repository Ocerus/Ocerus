#!/bin/bash
if [ `basename \`pwd\`` != "tools" ]; then
	echo "The script must be run from the tools/ directory"
	exit
fi
source ./common.sh


if [[ "$OS" == cygwin* ]]; then
	echo "\n\n"
	echo "------MAKE SURE YOU BUILD THE FOLLOWING PROJECTS FIRST:"
	echo "* documentation"
	echo "* Win32/_Ocerus in Release"
	echo "* Win32/_Ocerus in Deploy"
	echo "\n\n"
	#./buildVs9.bat
fi


cd ..

OUTPUT_DIR=../output

clearDirectory $OUTPUT_DIR/bin/Win32
clearDirectory $OUTPUT_DIR/shared
clearDirectory $OUTPUT_DIR/sources/cmake
clearDirectory $OUTPUT_DIR/sources/externalLibs
clearDirectory $OUTPUT_DIR/sources/src
clearDirectory $OUTPUT_DIR/sources/Win32


processDirectory Win32/bin/Release . $OUTPUT_DIR/bin/Win32 '.*Ocerus\.exe$\|.*Ocerus\.pdb$\|.*libexpat\.dll$'
processDirectory output . $OUTPUT_DIR/bin/Win32 '.*dbghelp\.dll$\|.*SDL\.dll$\|.*SILLY\.dll$\|.*SILLY_d\.dll$'
processDirectory output . $OUTPUT_DIR/shared '.*/data/.*\|.*/deploy/.*\|.*/test/.*' '.*ActionSave\.xml$'
processDirectory output/projects . $OUTPUT_DIR/shared/samples
processDirectory . src $OUTPUT_DIR/sources
processDirectory . cmake $OUTPUT_DIR/sources
processDirectory . . $OUTPUT_DIR/sources '.*CMakeLists.txt$'
processDirectory . Win32 $OUTPUT_DIR/sources '.*\.vcproj$\|.*\.sln$\|.*\.txt$\|.*\.wew$\|.*\.aps$\|.*\.rc$\|.*\.ico$\|.*\.h$' '.*/bin/.*'
LIBS_EXCLUDE=`excludeDirs obj tmp bin debug develop release`'.*\.vcproj\..+$\|'
processDirectory . externalLibs/angelscript $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE`excludeDir lib`
processDirectory . externalLibs/boost $OUTPUT_DIR/sources '.*'
processDirectory . externalLibs/Box2D $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE`excludeDir library`
processDirectory . externalLibs/cegui $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE'.*/lib/CEGUI.*\.lib$'
processDirectory . externalLibs/DbgLib $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE
processDirectory . externalLibs/dx9 $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE
processDirectory . externalLibs/expat $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE
processDirectory . externalLibs/ois $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE`excludeDir lib`
processDirectory . externalLibs/RTHProfiler $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE`excludeDir lib`
processDirectory . externalLibs/rudeconfig $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE`excludeDir lib`
processDirectory . externalLibs/SDL $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE
processDirectory . externalLibs/SOIL $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE`excludeDir lib`
processDirectory . externalLibs/UnitTest++ $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE
tools/documentation.sh $OUTPUT_DIR/shared/docs

processDirectory ../cd . $OUTPUT_DIR


cd tools