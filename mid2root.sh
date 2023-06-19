#!/bin/bash

commandline_args=("$@")
args="mid2root "

for arg in "${commandline_args[@]}"; do
    args=${args}" "${arg}
done

root.exe<<EOF
	dragon::Mid2Root("${args}");
EOF
