#!/bin/bash

RED=`tput setaf 1`
GREEN=`tput setaf 2`
RESET=`tput sgr0`

./testcases/$1 > /tmp/actual
code1=$?

./testcases/$1-expected > /tmp/expected
code2=$?

success=1

if [ ${code1} -eq ${code2} ]; then 
    diff /tmp/actual /tmp/expected >/dev/null 2>/dev/null || vimdiff /tmp/actual /tmp/expected
else
    success=0
    echo -ne "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
    echo -e "${RED}[\u2613] COMPILING $@ ... ${RESET}"
    echo -e "\tExpected: ${code2}\tActual: ${code1}"
fi

if [ ${success} -eq 1 ]; then
    echo -ne "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
    echo -e "${GREEN}[\u2713] COMPILING $@ ... ${RESET}"
fi
