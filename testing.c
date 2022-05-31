// C program to demonstrate snprintf()
#include <stdio.h>
#include <string.h>
int main()
{
    char buffer[50];
   
    // join two or more strings
    char* str1 = "quick";
    char* str2 = "brown";
    char* str3 = "lazy";
    int max_len = sizeof buffer;
    char buffer2[50] = "The ";

    int j = snprintf(buffer, max_len,
                 "The %s %s fox jumped over the %s dog.",
                 str1, str2, str3);
    
    strcat(buffer2, str1);
    strcat(buffer2, " ");
    strcat(buffer2, str2);
    strcat(buffer2, " ");
    strcat(buffer2, str3);
    printf("\nThe number of bytes printed to 'buffer' "
           "(excluding the null terminator) is %d\n",
           j);
    if (j >= max_len)
        fputs("Buffer length exceeded; string truncated",
              stderr);
    puts("Joined string:");
    puts(buffer);
    puts("anjaxsn");
    puts(buffer2);
 
    return 0;
}