#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}



@test "Handles nonexistent command" {
    run ./dsh <<EOF                
nonexistentcmd
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output based on actual observed behavior
    expected_output="execvp:Nosuchfileordirectorydsh2>Executionfailedwithexitcode1dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Echo special characters" {
    run ./dsh <<EOF
echo "Hello, \$USER!"
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="Hello,\$USER!dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Handles empty input (pressing Enter)" {
    run ./dsh <<EOF

EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh2>warning:nocommandsprovideddsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Handles multiple spaces between arguments" {
    run ./dsh <<EOF
echo     Hello      World
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="HelloWorlddsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Rejects running a plain file" {
    echo "This is a test file" > testfile.txt
    run ./dsh <<EOF
./testfile.txt
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expect "Permission denied" instead of "No such file or directory"
    expected_output="execvp:Permissiondenieddsh2>Executionfailedwithexitcode1dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    rm testfile.txt
}

@test "Rejects running an environment variable" {
    run ./dsh <<EOF
$HOME
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expect "Permission denied" instead of "No such file or directory"
    expected_output="execvp:Permissiondenieddsh2>Executionfailedwithexitcode1dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Rejects command with invalid special character" {
    run ./dsh <<EOF
ls *
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="ls:cannotaccess'*':NosuchfileordirectoryExecutionfailedwithexitcode2dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}