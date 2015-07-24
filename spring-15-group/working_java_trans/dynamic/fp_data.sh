#!/bin/bash


# check for polynomial
if [ -z "$1" ]; then
  echo usage: $0 poly
  exit
fi

# create test result folder
if [ ! -d test_result ]; then
    mkdir -p test_result;
fi

# create polynomial folder
if [ ! -d test_result/$1 ]; then
    mkdir -p "test_result/${1}";
fi


BITMASK=8
WINDOW=$[${#1}/2+4]
WINDOWMAX=$[WINDOW+20]
j=$WINDOW
k=$BITMASK

# loop through each file
for i in $(ls); do
    if [ $i != "fp" ] && [ $i != "fp_data.sh" ] && [ $i != "test_result" ] && [ $i != "nohup.out" ]; then
# create polynomial folder
	if [ ! -d "test_result/${1}/F${i}" ]; then
	    mkdir -p "test_result/${1}/F${i}";
	fi
	while [ $k -lt 14 ]; do
    	# loop through different bitmask	    
	    while [ $j -lt $WINDOWMAX ]; do
	    # loop through different windows
		if [ ! -d "test_result/${1}/F${i}/F${i}_W${j}" ]; then
		    mkdir -p "test_result/${1}/F${i}/F${i}_W${j}";
		fi
		echo "f = $i w = $j b = $k"
		./fp -i $1 -f $i -w $j -b $k &> "test_result/${1}/F${i}/F${i}_W${j}/F${i}_W${j}_B${k}" &
		
		let j+=4 
	    done
	    wait
	    let j=WINDOW
	    let k+=1
	done
	let k=BITMASK
    fi
done
