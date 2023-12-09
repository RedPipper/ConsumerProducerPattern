#!/bin/bash
#!/usr/bin/env sh

make
#secv run 
time=0
for i in {1..10}; do
  val=$(./Lab5 secv)
  time=$(($time + $val))
done
echo $(($time / 10))

time=0
cases=("2" "4" "12")
#multithr run

for testCase in "${cases[@]}"; do
  time=0
  for i in {1..10}; do
    val=$(./Lab5 par $testCase 4)
    time=$(($time + $val))
    sleep 1
  done
  echo "4 readers, $testCase writers" $(($time / 10))
done


