#!/bin/bash

USER=userClone.out
make TARGET=$USER > /dev/null 2>&1

echo "How many scenarios do you want to test?"
read Input
Counter=0
IsErr=False
while [ $Input -ne $Counter ]; do
	(( Counter++ ))
	./$USER > actionslog.txt 2> errlog.txt
	if [ -s errlog.txt ]; then
		echo "found a bug!"
		cat errlog.txt
		IsErr=True
		break
	fi
	rm -r Test
done

if [ $IsErr == False ]; then
	rm actionslog.txt
	echo "Didn't find any bug!"
	rm errlog.txt
fi
make clean TARGET=$USER > /dev/null
