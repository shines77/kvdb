
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include <iostream>
#include <iomanip>  // std::setw(), std::setfill(), std::setprecision().
#include <sstream>
#include <string>
#include <utility>

#include "kvdb/all.h"

// String compare mode
#define STRING_COMPARE_STDC     0
#define STRING_COMPARE_U64      1
#define STRING_COMPARE_SSE42    2

#define STRING_COMPARE_MODE     STRING_COMPARE_SSE42

using namespace kvdb;

#define CLIENT_PREFIX   "KVCache-127.0.0.1:7707> "

int main(int argc, char * argv[])
{
    printf("\n");
    printf("KVDB Client 1.0\n");
    printf("\n");

    bool exit = false;

    while (!exit) {
        printf("%s", CLIENT_PREFIX);
        unsigned char commands[1024];
        do {
#if defined(_WIN32)
            int cmd_len = scanf_s("%s", &commands, (int)(sizeof(commands) - 1));
#else
            int cmd_len = scanf("%s", &commands);
#endif
            if (cmd_len <= 0) continue;

            int first = 0, last = cmd_len - 1;
            unsigned char ch;

            // Find the first non space char position.
            while (ch = commands[first] != '\0') {
                if (likely(ch != ' ')) {
                    first++;
                    if (first >= cmd_len)
                        continue;
                }
                else {
                    break;
                }
            }

            // Find the last non space char position.
            while (last > 0) {
                ch = commands[last];
                if (likely(ch == ' ')) {
                    last--;
                }
                else {
                    break;
                }
            }

            commands[last] = '\0';

            unsigned char firstChar = commands[first];
            char * cmd = (char *)&commands[first];
            switch (firstChar) {
            case '\0':
                exit = true;
                break;
            case ' ':
                break;
            case 'b':
                if (strcmp(cmd, "bye") == 0) {
                    exit = true;
                }
                break;
            case 'q':
                if (strcmp(cmd, "quit") == 0) {
                    exit = true;
                }
                break;
            default:
                break;
            }
        } while (0);

        // Exit now ??
        if (exit) {
            printf("Byebye.\n");
            printf("\n");
            break;
        }
        
        // Print the response.
        printf("Ok.\n");
    }

    ::system("pause");
    return 0;
}
