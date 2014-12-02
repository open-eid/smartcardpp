#!/bin/sh

for (( i=0; i<$1; i++))
do
	open -a /Applications/Utilities/Terminal.app/Contents/MacOS/Terminal "$HOME/src/smartcardpp-cmake/build/Debug/smartcardpp-test"
	sleep 15
done
