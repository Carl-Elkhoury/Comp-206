#!/bin/bash

files=$(find $1 -type f -follow -iname mtl* -print | xargs ls -tr -R)

fileName=$(echo $1 | tr "/" "_")

convert -append ${files} ${fileName}.jpg

