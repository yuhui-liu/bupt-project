#!/bin/bash

mkdir -p output_for_semantic_errors

for testcase in ./semantic_errors/*.pas
do
  tempfile=$(mktemp)
  echo "2 1" > "$tempfile"
  cat "$testcase" >> "$tempfile"
  base_name=$(basename "$testcase" .pas)
  ./build/COMPILER < "$tempfile" 2> "output_for_semantic_errors/${base_name}.log"
  echo "Test case: $testcase"
done