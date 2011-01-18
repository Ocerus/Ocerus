#!/bin/bash
if [ `basename \`pwd\`` != "tools" ]; then
	echo "The script must be run from the tools/ directory"
	exit
fi
source ./common.sh


cd ..

OUTPUT_DIR=../release

clearDirectory $OUTPUT_DIR

if [[ "$OS" == cygwin* ]]; then
  processDirectory Win32/bin/Release . $OUTPUT_DIR '.*Ocerus\.exe$\|.*libexpat\.dll$'
  processDirectory output . $OUTPUT_DIR '.*dbghelp\.dll$\|.*SDL\.dll$\|.*SILLY\.dll$\|.*SILLY_d\.dll$'
else
  echo "NOT IMPLEMENTED!"
fi


processDirectory output . $OUTPUT_DIR '.*/data/.*\|.*/deploy/.*' '.*ActionSave\.xml$\|.*\.pdb$'
processDirectory output/projects/cube . $OUTPUT_DIR/samples/cube
tools/documentation.sh $OUTPUT_DIR/docs

cd tools