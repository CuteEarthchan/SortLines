#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define ALLOCATE_NUM 4096
#define BUFFER_SIZE 4096

// states and filenames
bool ignore_case = false;
bool file_output = false;
bool remove_empty_lines = false;
char* input_file_name;
char* output_file_name;

// argument strings
char* ignore_case_str = "--ignore-case";
char* remove_empty_lines_str = "--remove-empty-lines";

// used to hold a lower case version of the string while comparing
char* aLower;
char* bLower;

int comp( const void * a, const void * b )
{
    return strcmp( *(char * const *) a, *(char * const *) b );
}

int comp_ignore_case( const void * a, const void * b )
{
    size_t aLen = strlen( *(char * const *) a );
    size_t bLen = strlen( *(char * const *) b );

    for( size_t i = 0 ; i < aLen ; i++ )
    {
        aLower[i] = (char) tolower( (*(char * const *) a)[i] );
    }
    aLower[aLen] = '\0';

    for( size_t i = 0 ; i < bLen ; i++ )
    {
        bLower[i] = (char) tolower( (*(char * const *) b)[i] );
    }
    bLower[bLen] = '\0';

    return strcmp( (char * const) aLower, (char * const) bLower );
}

void parse_arguments( int argc, char** argv )
{
    // "parse" arguments
    switch( argc )
    {
        case 1:
            printf("Usage: %s ([arguments]) <input-file.txt> (<output-file.txt>)\n\n", argv[0]);
            printf("Arguments:\n\t%s\n\t%s\n", ignore_case_str, remove_empty_lines_str);
            exit( 0 );

        case 2:
            input_file_name = argv[1];
            break;

        case 3:
            if( strcmp(argv[1], ignore_case_str) == 0 )
            {
                ignore_case = true;
                input_file_name = argv[2];
            }
            else if( strcmp(argv[1], remove_empty_lines_str) == 0 )
            {
                remove_empty_lines = true;
                input_file_name = argv[2];
            }
            else
            {
                file_output = true;
                input_file_name = argv[1];
                output_file_name = argv[2];
            }
            break;

        case 4:
            if( strcmp(argv[1], ignore_case_str) == 0 )
            {
                ignore_case = true;

                if( strcmp(argv[2], remove_empty_lines_str) == 0 )
                {
                    remove_empty_lines = true;
                    input_file_name = argv[3];
                }
                else
                {
                    file_output = true;
                    input_file_name = argv[2];
                    output_file_name = argv[3];
                }
            }
            else if( strcmp(argv[1], remove_empty_lines_str) == 0 )
            {
                remove_empty_lines = true;
                if( strcmp(argv[2], ignore_case_str) == 0 )
                {
                    ignore_case = true;
                    input_file_name = argv[3];
                }
                else
                {
                    file_output = true;
                    input_file_name = argv[2];
                    output_file_name = argv[3];
                }
            }
            else
            {
                fprintf( stderr, "Invalid arguments!\n" );
                exit( 1 );
            }
            break;

        case 5:
            if( strcmp(argv[1], ignore_case_str) == 0 )
            {
                ignore_case = true;

                if( strcmp(argv[2], remove_empty_lines_str) == 0 )
                {
                    remove_empty_lines = true;
                    file_output = true;
                    input_file_name = argv[3];
                    output_file_name = argv[4];
                }
                else
                {
                    fprintf( stderr, "Invalid arguments!\n" );
                    exit( 1 );
                }
            }
            else if( strcmp(argv[1], remove_empty_lines_str) == 0 )
            {
                remove_empty_lines = true;
                if( strcmp(argv[2], ignore_case_str) == 0 )
                {
                    ignore_case = true;
                    file_output = true;
                    input_file_name = argv[3];
                    output_file_name = argv[4];
                }
                else
                {
                    fprintf( stderr, "Invalid arguments!\n" );
                    exit( 1 );
                }
            }
            else
            {
                fprintf( stderr, "Invalid arguments!\n" );
                exit( 1 );
            }
            break;

        default:
        {
            fprintf( stderr, "Invalid arguments!\n" );
            exit( 1 );
        }
    }
}

int main( int argc, char** argv )
{
    parse_arguments( argc, argv );


    // open file
    FILE * file = fopen(input_file_name, "r");
    if( file == NULL )
    {
        fprintf(stderr, "File '%s' could not be opened!\n", input_file_name);
        return 1;
    }

    // array of lines
    size_t allocated_line_pointers = ALLOCATE_NUM; //ALLOCATE_NUM;
    char** lines = malloc( sizeof(char*) * allocated_line_pointers );

    size_t line_count = 0;

    // buffer for reading the file
    char* line_buffer = malloc( sizeof(char) * BUFFER_SIZE );


    while( fgets(line_buffer, sizeof(char) * BUFFER_SIZE, file) != NULL ) // read line by line
    {
        if( allocated_line_pointers - line_count == 0 ) // allocate more pointers if no more left
        {
            allocated_line_pointers += ALLOCATE_NUM;
            lines = (char**) realloc( lines, sizeof(char*) * allocated_line_pointers );
        }

        // replacing the newline with an end of string
        size_t len = strlen( line_buffer );
        line_buffer[len - 1] = '\0';

        // allocate exact fitting amount of memory and copy the string from buffer to array
        lines[line_count] = malloc( sizeof(char) * len );
        strcpy( lines[line_count], (const char*) line_buffer );

        line_count++;
    }


    // close the file and free the buffer
    fclose( file );
    free( file );
    free( line_buffer );


    // sort the lines
    if( ignore_case )
    {
        aLower = malloc( sizeof(char) * BUFFER_SIZE );
        bLower = malloc( sizeof(char) * BUFFER_SIZE );

        qsort( lines, line_count, sizeof(char*), comp_ignore_case );

        free( aLower );
        free( bLower );
    }
    else
    {
        qsort( lines, line_count, sizeof(char*), comp);
    }


    // opens an output file or sets output to stdout
    if( file_output )
    {
        file = fopen(output_file_name, "w");
        if( file == NULL )
        {
            fprintf(stderr, "File '%s' could not be opened!\n", output_file_name);
            return 1;
        }
    }
    else
    {
        file = stdout;
    }


    // prints output with or without empty lines
    if( remove_empty_lines )
    {
        for( size_t i = 0 ; i < line_count ; i++ )
        {
            if( strcmp( lines[i], "" ) )
            {
                fprintf(file, "%s\n", lines[i]);
            }
        }
    }
    else
    {
        for( size_t i = 0 ; i < line_count ; i++ )
        {
            fprintf(file, "%s\n", lines[i]);
        }
    }


    // closes the output file
    if( file_output )
    {
        fclose( file );
        free( file );

        printf("File was successfully saved at '%s'.\n", output_file_name);
    }


    // free the array
    for( size_t i = 0 ; i < line_count ; i++ )
    {
        free( lines[i] );
    }
    free( lines );

    return 0;
}
