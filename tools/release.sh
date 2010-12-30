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



CUR_DIR_NAME=`basename \`pwd\``
if [ "$CUR_DIR_NAME" != "tools" ]; then
	echo "The script must be run from the tools/ directory"
	exit
fi


cd ..

OUTPUT_DIR=../release

clearDirectory $OUTPUT_DIR

OS=`uname | tr [A-Z] [a-z]`
if [[ "$OS" == cygwin* ]]; then
  processDirectory Win32/bin/Release . $OUTPUT_DIR '.*Ocerus\.exe$\|.*libexpat\.dll$'
  processDirectory output . $OUTPUT_DIR '.*dbghelp\.dll$\|.*SDL\.dll$\|.*SILLY\.dll$\|.*SILLY_d\.dll$'
else
  echo "NOT IMPLEMENTED!"
fi


processDirectory output . $OUTPUT_DIR '.*/data/.*\|.*/deploy/.*' '.*ActionSave\.xml$\|.*\.pdb$'
processDirectory output/projects/cube . $OUTPUT_DIR/samples/cube


cd tools