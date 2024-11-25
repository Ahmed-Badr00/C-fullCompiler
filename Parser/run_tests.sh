#!/bin/bash

# List of code files to test
code_files=(
    "valid_simple.c"
    "valid_if_else.c"
    "valid_while_loop.c"
    "valid_compound.c"
    "valid_nested_declarations.c"
    "valid_declarations_after_statements.c"
    "invalid_missing_semicolon.c"
    "invalid_undefined_keyword.c"
    "invalid_missing_brace.c"
    "invalid_expression.c"
    "invalid_missing_then.c"
    "invalid_extra_semicolon.c"
)

# Output file
output_file="parser_output.txt"

# Remove the output file if it exists
if [ -f "$output_file" ]; then
    rm "$output_file"
fi

echo "Starting parser tests..." | tee -a "$output_file"
echo "=========================" >> "$output_file"
echo "" >> "$output_file"

# Run the parser on each code file and append the output to the output file
for code_file in "${code_files[@]}"; do
    echo "Running parser on $code_file..." | tee -a "$output_file"
    echo "----- Output for $code_file -----" >> "$output_file"
    ./parser "$code_file" >> "$output_file" 2>&1
    echo "" >> "$output_file"
    echo "---------------------------------" >> "$output_file"
    echo "" >> "$output_file"
done

echo "All runs completed. Output saved to $output_file."

