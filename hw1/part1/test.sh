#!/bin/bash
count=0
for ((i=$1;i<=$2;i++))
do
	./myexp -s $i | grep -o Passed > /dev/null 2>&1
	if [ $? -eq 0 ]; then
		count=$(($count+1))
	fi
done
if [ $count -eq $(($2-$1+1)) ]; then
	echo "All Pass"
fi

