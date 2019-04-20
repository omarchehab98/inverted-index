#include <stdio.h>
#include <errno.h>
#include "hash_map_lib.h"

char *usage = "usage: %s [file]\n";

/*
## EBNF

```
prog ::= [ cmd ]
cmd  ::= set | get | del | prt
set  ::= "SET" arg arg
get  ::= "GET" arg
del  ::= "DEL" arg
prt  ::= "PRT"
```

## FSM

Initial State: SPACESO(CMD)

| State          | Read      | Next State           |
|----------------|-----------|----------------------|
| SPACES(*next)  | \s        | SPACESO(next)        |
|                |           |                      |
| SPACESO(CMD)   | \eof      | EXIT(0)              |
| SPACESO(*next) | \s        | SPACESO(next)        |
| SPACESO(*next) |           | *next                |
|                |           |                      |
| CMD            | SET       | SPACES(ARG0(SET))    |
| CMD            | GET       | SPACES(ARG0(GET))    |
| CMD            | DEL       | SPACES(ARG0(DEL))    |
| CMD            | PRT       | EXEC                 |
|                |           |                      |
| ARG0(SET)      | [^\s\eof] | ARG0(SET)            |
| ARG0(SET)      | \s        | SPACESO(ARG1(SET))   |
| ARG1(SET)      | [^\s\eof] | ARG1(SET)            |
| ARG1(SET)      |           | EXEC                 |
|                |           |                      |
| ARG0(GET)      | [^\s\eof] | ARG0(GET)            |
| ARG0(GET)      |           | EXEC                 |
|                |           |                      |
| ARG0(DEL)      | [^\s\eof] | ARG0(DEL)            |
| ARG0(DEL)      |           | EXEC                 |
|                |           |                      |
| EXEC           |           | SPACESO(CMD)         |
|                |           |                      |
|                | \eof      | EXIT(1)              |
*/

#define CMD_LEN 3
#define ARG_LEN 255

#define STATE_SPACES 0
#define STATE_SPACESO 1
#define STATE_CMD 2
#define STATE_ARG0 3
#define STATE_ARG1 4
#define STATE_EXEC 5


int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}


int is_arg(char c) {
    return !is_space(c) && c != EOF;
}


char *clone_string(char *key) {
    char *clone = malloc(strlen(key));
    if (clone == NULL) return NULL;
    strcpy(clone, key);
    return clone;
}

void hash_map_print_value_string(map_value_t value) {
    printf("\"%s\"", value);
}

int main(int argc, char **argv) {
    if (argc > 2) printf(usage, argv[0]), exit(1);

    FILE *fp = stdin;
    if (argc == 2) fp = fopen(argv[1], "r");
    if (fp == NULL) goto err;

    map_t map = new_hash_map();
    if (map == NULL) goto err;

    int state = STATE_SPACESO;
    int next = STATE_CMD;

    char cmd[CMD_LEN + 1];
    char arg0[ARG_LEN + 1];
    char arg1[ARG_LEN + 1];

    char c;
    short i = 0;
    while (1) {
        c = fgetc(fp);

        if (state == STATE_SPACESO && next == STATE_CMD && c == EOF) {
            exit(0);
        }

        if (state == STATE_SPACESO && !is_space(c)) {
            state = next;
        }

        switch (state) {
            case STATE_SPACES: {
                if (is_space(c)) {
                    state = STATE_SPACESO;
                } else {
                    errno = EFTYPE;
                    goto err;
                }
                break;
            }

            case STATE_CMD: {
                if (is_arg(c) && i < CMD_LEN) {
                    cmd[i++] = c;

                    if (i == CMD_LEN) {
                        cmd[i] = '\0';
                        i = 0;
                        if (strcmp(cmd, "PRT") == 0) {
                            state = STATE_EXEC;
                        } else {
                            state = STATE_SPACES;
                            next = STATE_ARG0;
                        }
                    }
                } else {
                    errno = EFTYPE;
                    goto err;
                }
                break;
            }

            case STATE_ARG0: {
                if (strcmp(cmd, "SET") == 0) {
                    if (is_arg(c) && i < ARG_LEN) {
                        arg0[i++] = c;
                    } else {
                        state = STATE_SPACESO;
                        next = STATE_ARG1;
                        arg0[i] = '\0';
                        i = 0;
                    }
                } else {
                    if (is_arg(c) && i < ARG_LEN) {
                        arg0[i++] = c;
                    } else {
                        state = STATE_EXEC;
                        arg0[i] = '\0';
                        i = 0;
                    }
                }
                break;
            }

            case STATE_ARG1: {
                if (is_arg(c) && i < ARG_LEN) {
                    arg1[i++] = c;
                } else {
                    state = STATE_EXEC;
                    arg1[i] = '\0';
                    i = 0;
                }
                break;
            }
        }

        if (state == STATE_EXEC) {
            if (strcmp(cmd, "SET") == 0) {
                map = hash_map_set(map, arg0, clone_string(arg1));
                if (map == NULL) goto err;
            } else if (strcmp(cmd, "GET") == 0) {
                char *value = hash_map_get(map, arg0);
                printf("%s: %s\n", arg0, value);
            } else if (strcmp(cmd, "DEL") == 0) {
                hash_map_del(map, arg0);
            } else if (strcmp(cmd, "PRT") == 0) {
                hash_map_print(map, 0, &hash_map_print_value_string);
            } else {
                errno = EFTYPE;
                goto err;
            }
            state = STATE_SPACESO;
            next = STATE_CMD;
        }
    }

    return 0;

    err:
        perror(argv[0]);
        return 1;
}
