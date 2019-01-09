#!/bin/bash
tr "A-Z" "$(cut -f2 -d$'\n' $1)" < $2 | tr "a-z" "$(cut -f1 -d$'\n' $1)"