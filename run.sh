#!/bin/bash
sh make.sh
sh comp.sh
./program $1
echo ' '
echo $?