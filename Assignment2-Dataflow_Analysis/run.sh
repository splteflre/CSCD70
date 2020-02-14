#!/bin/bash

CODE="$(pwd)"

docker run -it --rm -v $CODE:/a2 --name CSCD70_A1 llvm:6.0 

