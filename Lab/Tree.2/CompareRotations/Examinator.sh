#!/bin/bash

source ./Configs/config.ini
source ./Configs/config.mk

MakerExe=inputmaker.out

cd ./Exam
make > /dev/null
cp "$ExamExe" ../

cd ../Test

make > /dev/null
cp "$TestExe" ../
cd ..

g++ input_maker.cpp -o input_maker.out

echo "Enter PassRoof"
read PassRoof
Counter=0
UserInput=Continue

while [ "$UserInput" != "Exit" ]; do
	if [ "$UserInput" == "Continue" ]; then
		Input="$(./input_maker.out)"
		ExamOutput=$(echo "$Input" | ./$ExamExe)
		TestOutput=$(echo "$Input" | ./$TestExe)
		if [ "$ExamOutput" != "$TestOutput" ]; then
			mkdir -p Logs
			echo "$ExamOutput" >> Logs/exam_log.txt
			echo ---------------------------------------------------- >> Logs/exam_log.txt
			echo "$TestOutput" >> Logs/test_log.txt
			echo ---------------------------------------------------- >> Logs/test_log.txt
			echo "$Input" >> Logs/input_log.txt
			echo ---------------------------------------------------- >> Logs/input_log.txt
			echo "Failed! (Exit/Continue)"
			read UserInput
		elif [ $Counter -eq $PassRoof ]; then
			echo "Pass roof reached! (Exit/Continue)"
			read UserInput
		else
			((Counter++))
		fi
	else
		echo "Invalid Input!"
		read UserInput
	fi
done

rm -r $BaseDir
rm *.out
