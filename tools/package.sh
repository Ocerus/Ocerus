cd ..

function processDirectory {
	# copies the directory into the output
	# parameters: source destination inclusionRegex=".*" exclusionRegex=""
	if [ $# -lt 3 ]; then
		INC_REGEX='.*'
	else
		INC_REGEX=$3
	fi
	
	if [ $# -lt 4 ]; then
		EXC_REGEX='.*\.svn\(/.*\)?'
	else
		EXC_REGEX='\(.*\.svn\(/.*\)?\|'$4'\)'
	fi
	
	echo "Processing $1 to $2 included $INC_REGEX excluded $EXC_REGEX";
	
	rm -rf "$2/$1"
	
	NEW_DIRS=`find "$1" -type d -iregex "$INC_REGEX" -and -not -iregex "$EXC_REGEX" -printf "$2/%p " `
	FILES=`find "$1" -type f -iregex "$INC_REGEX" -and -not -iregex "$EXC_REGEX"`
	mkdir -p $NEW_DIRS
	cp --parents $FILES $2
}


OUTPUT_DIR=../output

processDirectory src $OUTPUT_DIR/source


cd tools