#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file
#!/usr/bin/env bats

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}


setup() {
    # Setup necessary environment or variables
    touch temp.txt
}

teardown() {
    # Clean up test artifacts
    rm -f temp.txt
}

@test "List directory contents" {
    run ./dsh "ls"
    [ "$status" -eq 0 ]
    echo "$output" | grep temp.txt
}

@test "Change directory" {
    mkdir -p test_dir
    run ./dsh "cd test_dir"
    [ "$status" -eq 0 ]
    run pwd
    [ "${output##*/}" = "test_dir" ]
    cd ..
    rmdir test_dir
}

@test "Environment variable creation and access" {
    run ./dsh "export TEST_VAR=hello"
    [ "$status" -eq 0 ]
    run ./dsh "echo \$TEST_VAR"
    [ "$output" = "hello" ]
}

@test "Execute external script" {
    echo "echo hello world" > test_script.sh
    chmod +x test_script.sh
    run ./dsh "./test_script.sh"
    [ "$status" -eq 0 ]
    [ "$output" = "hello world" ]
    rm test_script.sh
}

@test "Handling background processes" {
    run ./dsh "sleep 5 &"
    [ "$status" -eq 0 ]
    run pgrep sleep
    [ "$status" -eq 0 ]
}

@test "Using pipes" {
    run ./dsh "echo 'hello' | tr 'h' 'H'"
    [ "$status" -eq 0 ]
    [ "$output" = "Hello" ]
}

@test "Check file write and read" {
    run ./dsh "echo 'hello' > temp.txt"
    [ "$status" -eq 0 ]
    run cat temp.txt
    [ "$output" = "hello" ]
}

@test "Start server command" {
    run ./dsh "start-server"
    [ "$status" -eq 0 ]
    [ "$output" = "Server started successfully" ]
}

@test "Stop server command" {
    run ./dsh "stop-server"
    [ "$status" -eq 0 ]
    [ "$output" = "Server stopped successfully" ]
}

@test "Restart server command" {
    run ./dsh "restart-server"
    [ "$status" -eq 0 ]
    [ "$output" = "Server restarted successfully" ]
}