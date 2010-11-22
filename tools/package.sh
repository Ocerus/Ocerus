#!/bin/bash
set -e

function clearDirectory {
	echo "Removing `pwd`/$1"
	rm -rf `pwd`/"$1"
}

function processDirectory {
	# copies the directory into the output
	# parameters: runDirectory source destination inclusionRegex=".*" exclusionRegex=""
	
	if [ $# -lt 4 ]; then
		INC_REGEX='.*'
	else
		INC_REGEX=$4
	fi
	
	if [ $# -lt 5 ]; then
		EXC_REGEX='.*\.svn\(/.*\)?'
	else
		EXC_REGEX='\(.*\.svn\(/.*\)?\|'$5'\)'
	fi
	
	echo "Changing to $1 and processing $2 to $3 included $INC_REGEX excluded $EXC_REGEX"
	DEST=`pwd`/$3
	
	cd "$1"	
	
	NEW_DIRS=`find "$2" -type d -iregex "$INC_REGEX" -and -not -iregex "$EXC_REGEX" -printf "$DEST/%p " `
	FILES=`find "$2" -type f -iregex "$INC_REGEX" -and -not -iregex "$EXC_REGEX"`
	
	mkdir -p "$DEST"
	if [ -n "$NEW_DIRS" ]; then
		mkdir -p $NEW_DIRS
	fi
	
	if [ -z "$FILES" ]; then
		echo "No files to copy"
	else
		cp --parents $FILES $DEST
	fi
	
	cd - > /dev/null
}

function excludeDir {
	echo -n '.*/'"$1"'\(/.*\)?$'
}

function excludeDirs {
	for dir in "$@"; do
		echo -n '.*/'"$dir"'\(/.*\)?$\|'
	done
}


cd ..
CUR_DIR_NAME=`basename \`pwd\``
if [ "$CUR_DIR_NAME" != "trunk" ]; then
	echo "The script must be run from the tools/ directory"
	exit
fi


OUTPUT_DIR=../output


clearDirectory $OUTPUT_DIR/bin/Win32
clearDirectory $OUTPUT_DIR/doc
clearDirectory $OUTPUT_DIR/shared
clearDirectory $OUTPUT_DIR/sources/cmake
clearDirectory $OUTPUT_DIR/sources/externalLibs
clearDirectory $OUTPUT_DIR/sources/src
clearDirectory $OUTPUT_DIR/sources/Win32


processDirectory Win32/bin/Develop . $OUTPUT_DIR/bin/Win32 '.*Ocerus\.exe$\|.*Ocerus\.pdb$\|.*libexpat\.dll$'
processDirectory output . $OUTPUT_DIR/bin/Win32 '.*dbghelp\.dll$\|.*SDL\.dll$\|.*SILLY\.dll$\|.*SILLY_d\.dll$'
processDirectory . doc/doxygen $OUTPUT_DIR '.*/html/.*$\|.*index\.html$\|.*mainpage\.txt$\|.*Ocerus\.dox$'
processDirectory . doc/userDocumentation $OUTPUT_DIR '.*Documentation\.pdf$'
processDirectory output . $OUTPUT_DIR/shared '.*/data/.*\|.*/deploy/.*\|.*/test/.*' '.*ActionSave\.xml$'
processDirectory output/projects . $OUTPUT_DIR/shared/samples
processDirectory . src $OUTPUT_DIR/sources
processDirectory . cmake $OUTPUT_DIR/sources
processDirectory . . $OUTPUT_DIR/sources '.*CMakeLists.txt$'
processDirectory . Win32 $OUTPUT_DIR/sources '.*\.vcproj$\|.*\.sln$\|.*\.txt$\|.*\.wew$' '.*/bin/.*'
LIBS_EXCLUDE=`excludeDirs obj tmp bin debug develop release`'.*\.vcproj\..+$\|'
processDirectory . externalLibs/angelscript $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE`excludeDir lib`
processDirectory . externalLibs/boost $OUTPUT_DIR/sources '.*' $LIBS_EXCLUDE
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

processDirectory ../cd . $OUTPUT_DIR


cd tools