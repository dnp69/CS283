#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int);
int reverse_string(char*, int);
int word_print(char*, int);

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    char *src = user_str;
    char *dst = buff; 
    int char_count = 0;    
    int in_whitespace = 1;     
    int bytes_used = 0;        

    while (*src != '\0') {
        if (*src == ' ' || *src == '\t') {
            if (!in_whitespace) {
                if (bytes_used >= len) return -1;
                *dst++ = ' ';
                bytes_used++;
                char_count++;
                in_whitespace = 1;
            }
        } else {
            if (bytes_used >= len) return -1;
            *dst++ = *src;
            bytes_used++;
            char_count++;
            in_whitespace = 0;
        }
        src++;
    }

    if (in_whitespace && char_count > 0) {
        dst--;
        bytes_used--;
        char_count--;
    }

    while (bytes_used < len) {
        *dst++ = '.';
        bytes_used++;
    }

    return char_count;
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff + i));
    }
    printf("]\n");
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int count_words(char *buff, int str_len){
    //YOU MUST IMPLEMENT
    int word_count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        if (*(buff + i) == ' ') {
            if (in_word) {
                word_count++;
                in_word = 0;
            }
        } else {
            in_word = 1;
        }
    }

    if (in_word) word_count++;
    return word_count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int reverse_string(char *buff, int str_len) {
    int start = 0;
    int end = str_len - 1;

    while (buff[end] == '.') end--;
    while (start < end) {
        char tmp = *(buff + start);
        *(buff + start) = *(buff + end);
        *(buff + end) = tmp;
        start++;
        end--;
    }
    return 0;
}

int word_print(char *buff, int str_len) {
    int word_count = 0;
    int in_word = 0;
    int word_start = 0;
    int i = 0;

    printf("Word Print\n----------\n");
    while (i < str_len && *(buff + i) != '.') {
        char c = *(buff + i);
        if (c == ' ') {
            if (in_word) {
                int word_len = i - word_start;
                word_count++;
                printf("%d. ", word_count);
                for (int j = word_start; j < i; j++) {
                    putchar(*(buff + j));
                }
                printf("(%d)\n", word_len);
                in_word = 0;
            }
        } else {
            if (!in_word) {
                in_word = 1;
                word_start = i;
            }
        }
        i++;
    }

    if (in_word) {
        int word_len = i - word_start;
        word_count++;
        printf("%d. ", word_count);
        for (int j = word_start; j < i; j++) {
            putchar(*(buff + j));
        }
        printf("(%d)\n", word_len);
    }
    printf("\nNumber of words returned: %d\n", word_count);
    return word_count;
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /*
    This is safe because the condition if ((argc < 2) || (*argv[1] != '-')) ensures the program will not attempt to access argv[1] unless it exists. 
    If fewer than two arguments are provided (argc < 2), the program prints usage instructions and exits. 
    This prevents undefined behavior and ensures the program only continues with valid input.
    */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /*
    The statement if (argc < 3) ensures that the user has provided a string argument along with the option flag. 
    If the string is missing, the program exits safely after displaying the usage instructions. 
    This avoids accessing argv[2] when it doesn't exist, preventing runtime errors.
    */

    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    buff = (char *) malloc(BUFFER_SZ * sizeof(char));
    if (!buff) {
        printf("Error: Memory allocation failed\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos     
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, user_str_len);  //you need to implement  
            if (rc < 0){
                printf("Error counting words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options

        case 'r':
            rc = reverse_string(buff, user_str_len);
            if (rc < 0) {
                printf("Error reversing string, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            break;

        case 'w':
            rc = word_print(buff, user_str_len);
            if (rc < 0) {
                printf("Error printing words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            break;

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff, BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
/*
Providing both the buffer pointer and its length to functions is important for safety and flexibility. 
It ensures the function processes only the allocated memory, avoiding buffer overflows and potential crashes. 
Even though the buffer size is fixed now, passing the length allows the code to adapt easily if the size changes in the future. 
This approach makes the program more reliable and easier to read or update later.
*/