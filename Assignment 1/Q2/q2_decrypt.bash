#!/bin/bash
tr "$(cut -f2 -d$'\n' $1)" "A-Z"  < $2 | tr "$(cut -f1 -d$'\n' $1)" "a-z"