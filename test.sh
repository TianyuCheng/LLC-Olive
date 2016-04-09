#!/bin/bash

RED=`tput setaf 1`
GREEN=`tput setaf 2`
RESET=`tput sgr0`

./testcases/$1.actual > /tmp/actual
code1=$?

./testcases/$1.expected > /tmp/expected
code2=$?

success=1

if [ ${code1} -eq ${code2} ]; then 
    DIFF=$(diff /tmp/actual /tmp/expected)
    if [ "$DIFF" != "" ]; then
        success=0
        vim /tmp/expected -c "vert diffsplit /tmp/actual"
        echo -ne "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
        echo -e "${RED}[\u2613] TESTING $@ ... ${RESET}"
    fi
else
    success=0
    echo -ne "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
    echo -e "${RED}[\u2613] TESTING $@ ... ${RESET}"
    echo -e "\tExpected: ${code2}\tActual: ${code1}"
fi

if [ ${success} -eq 1 ]; then
    echo -ne "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
    echo -e "${GREEN}[\u2713] TESTING $@ ... ${RESET}"
fi
