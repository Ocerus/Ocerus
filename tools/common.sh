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

function processFile {
	# copies the single file into the output file
	# parameters: sourceFile destinationFile
	
	echo "Processing $1 to $2."
	DEST=`pwd`/$2
	cp "$1" "$DEST"
}

function excludeDir {
	echo -n '.*/'"$1"'\(/.*\)?$'
}

function excludeDirs {
	for dir in "$@"; do
		echo -n '.*/'"$dir"'\(/.*\)?$\|'
	done
}


OS=`uname | tr [A-Z] [a-z]`
