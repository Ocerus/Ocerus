#!/bin/bash
MUST_LEAVE_TOOLS=false
COMMON_PATH=./tools/common.sh
if [ `basename \`pwd\`` == "tools" ]; then
	MUST_LEAVE_TOOLS=true
  COMMON_PATH=./common.sh
fi
source "$COMMON_PATH"


# Packages the documentation files into the destination folder.
if [ $# -lt 1 ]; then
  echo "Not enough parameters.";
  exit;
fi
OUTPUT_DIR=$1;


if $MUST_LEAVE_TOOLS; then
  cd ..
fi


mkdir -p "$OUTPUT_DIR"

processFile "doc/designDocumentation/DesignDocumentation.pdf" "$OUTPUT_DIR/DesignDocumentation.pdf"
processFile "doc/extendingOcerus/Extending Ocerus.pdf" "$OUTPUT_DIR/ExtendingOcerus.pdf"
processFile "doc/shortcuts/Shortcuts.pdf" "$OUTPUT_DIR/Shorcuts.pdf"
processFile "doc/userGuide/userGuide.pdf" "$OUTPUT_DIR/UserGuide.pdf"
processDirectory "doc/userGuide" "archive" $OUTPUT_DIR
processDirectory "doc/doxygen" . $OUTPUT_DIR '.*/Doxygen/.*$\|.*Doxygen\.html$'
processDirectory "doc/scriptReference" . $OUTPUT_DIR '.*/ScriptReference/.*$\|.*ScriptReference\.html$'


if $MUST_LEAVE_TOOLS; then
  cd tools
fi
