#!/usr/bin/env bash

if [ -z "$1" ]; then
  echo "Enter the level please"
  exit 1
fi

level=$1

# Handle bonus levels
if [ "$level" -eq 0 ]; then
  sshpass -p "level0" scp -P 2222 -r level0@localhost:~ ./level0_home/
  exit 0
elif [ "$level" -eq 10 ]; then
  level_name="bonus0"
  flag_source="level9"
elif [ "$level" -eq 11 ]; then
  level_name="bonus1"
  flag_source="bonus0"
elif [ "$level" -eq 12 ]; then
  level_name="bonus2"
  flag_source="bonus1"
elif [ "$level" -eq 13 ]; then
  level_name="bonus3"
  flag_source="bonus2"
else
  # Regular levels (0-9)
  level_name="level$level"
  level_flag=$(( $level - 1 ))
  flag_source="level$level_flag"
fi

sshpass -p "$(cat ./$flag_source/flag)" scp -P 2222 -r $level_name@localhost:~ ./${level_name}_home/