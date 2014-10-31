#!/bin/sh

for (( i=0; i<5; i++))
do
	open -a /Applications/Utilities/Terminal.app/Contents/MacOS/Terminal "$HOME/src/smartcardpp_cmake/build/Debug/smartcardpp-test"
done