#!/bin/sh

output="./output.txt"
source="./source.json"
curl $1 | base64 -d > $output


if [ -s $output ]; then
    echo '' > $source
    while  read -r line ; do
	echo ${line:8} | base64 -d >> $source
	echo >> $source
    done < $output
else
    echo "./output.txt is empty"
fi

./subtrans -p source.json
