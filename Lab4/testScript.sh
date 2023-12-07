#!/bin/bash
#!/usr/bin/env sh

make
#secv run 
time=0
for i in {1..10}; do
  val=$(./Lab4 secv)
  time=$(($time + $val))
done
echo $(($time / 10))

time=0
cases=("4" "6" "8" "16")
#multithr run
for pr in {1..2}; do
  for testCase in "${cases[@]}"; do 
    time=0
    for i in {1..10}; do 
      val=$(./Lab4 par $testCase $pr)
      time=$(($time + $val))
    done
    echo "$pr readers, $testCase writers" $(($time / 10))
  done
done

