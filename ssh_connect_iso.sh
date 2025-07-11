#!/usr/bin/env bash

if [ -z "$1" ]; then
  echo "Enter the level please}"
  exit 1
fi

ssh level$1@localhost -p 2222
