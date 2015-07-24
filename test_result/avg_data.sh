#!/bin/bash

# check for polynomial
if [ -z "$1" ]; then
  echo usage: $0 poly folder
  exit
fi

Files=$1/*

# test_data
for f in $Files; do
    Windows=$f/*
    for w in $Windows; do
	bitmask=$w/*
	if [ -e "${w}${w#$f}_avg" ]; then
	    rm "${w}${w#$f}_avg"
	fi
	for b in $bitmask; do
	    if [ $f != "avg" ] && [ $f != "nohup.out" ]; then
		printf "%s %s\n" `awk 'BEGIN{count=0;sum=0;} {count++; sum+=$4} END{print sum/count}' $b` "${b##*B}"  &>> "${w}${w#$f}_avg"
	    fi
	done
    done
done
