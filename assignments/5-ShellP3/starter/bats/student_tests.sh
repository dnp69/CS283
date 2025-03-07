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

@test "Run a simple command: echo" {
    run ./dsh <<EOF
echo hello
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "hello" ]]
}

@test "Run a piped command: ls | grep .c" {
    run ./dsh <<EOF
ls | grep ".c"
EOF
    [ "$status" -eq 0 ]
}

@test "Pipeline with multiple commands: ps aux | grep bash | wc -l" {
    run ./dsh <<EOF
ps aux | grep bash | wc -l
EOF
    [ "$status" -eq 0 ]
}

@test "Test no input (just enter)" {
    run ./dsh <<EOF

EOF
    [ "$status" -eq 0 ]
}

@test "Test invalid command" {
    run ./dsh <<EOF
drexeldragon
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "drexeldragon" ]]
}

@test "Test exit command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "exiting..." ]]
}

@test "Check multiple commands piped together" {
    run ./dsh <<EOF
echo -e "Mario\nDragon\nArch\nChestnut" | grep Dragon | wc -l
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "1" ]]
}

@test "Long pipeline of four commands" {
    run ./dsh <<EOF
echo -e "Lancaster\nDrexel\nLibrary\nMarket" | grep l | sort | wc -l
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "3" ]]
}

@test "Whitespace handling in pipeline" {
    run ./dsh <<EOF
   echo    hello   |    tr a-z A-Z  
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "HELLO" ]]
}
