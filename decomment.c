/*--------------------------------------------------------------------*/
/* decomment.c                                                        */
/* Author: Kevin Tran                                                 */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>
/* defines constants representing each state in the DFA */
enum Statetype {NORMAL, MAYBE_COMMENT_START, COMMENT_START,
    MAYBE_COMMENT_END, CHAR_START, CHAR_ESCAPE, STRING_START, 
    STRING_ESCAPE};

/* Implements the NORMAL state of the DFA. c is the current DFA 
   character. Writes c to stdout unless it is a / maybe beginning a 
   comment. Return the next DFA state. */
handleNormalState(int c) {
    enum Statetype state;
     /* could be a comment as long as it's not the last char */
    if (c== '/') {
        /* replace char with nothing, must see if it's a 
        comment first */
        state = MAYBE_COMMENT_START;
    } else if (c == '\'') {
        state = CHAR_START;
        putchar(c);
    } else if (c == '\"') {
        state = STRING_START;
        putchar(c);
    }
    else {
        state = NORMAL;
        putchar(c);
    }
    
    return state;
}
/* Implements the MAYBE_COMMENT_START state of the DFA. c is the current
   DFA character. Writes c to stdout unless it is a * beginning a 
   comment. Writes / and c if it is a character not ' or ". Return the 
   next DFA state. */
handleMaybeCommentStartState(int c) {
    enum Statetype state;
    if (c == '/') { 
        state = MAYBE_COMMENT_START;
        /* should put down another / since one is 
        missing from earlier */
        putchar(c);
    } else if (c == '*') {
        state = COMMENT_START;
        /* comment marker found, replace with a space */
        putchar(' ');
    } else if (c == '\'') {
        state = CHAR_START;
        /* not a comment so put back removed / */
        putchar('/'); 
        putchar(c);
    } else if (c == '\"') {
        state = STRING_START;
        /* not a comment so put back removed / */
        putchar('/'); 
        putchar(c);
    }
    else {
        /* not a comment so we must put back / from earlier */
        putchar('/');
        putchar(c);
        state = NORMAL;
    }
    return state;
}
/* Implements the COMMENT_START state of the DFA. c is the current
   DFA character. Writes a new line to stdout if c is a new line. Return
   the next DFA state. */
handleCommentStartState(int c) {
    enum Statetype state;
    if (c == '*') { /* could be the end of comment */
        state = MAYBE_COMMENT_END;
    } else if (c == '\n') { /* to handle multiline comments*/
        state = COMMENT_START;
        putchar('\n');
    } else { /* still in comment, keep putting down characters */
        state = COMMENT_START;
    }
    return state;
}
/* Implements the MAYBE_COMMENT_END state of the DFA. c is the current
   DFA character. Writes a new line to stdout if c is a new line. Return
   the next DFA state. */
handleMaybeCommentEndState(int c) {
    enum Statetype state;
    if (c == '*') { /* still possible to terminate comment, put down * 
        since we removed one earlier */
        state = MAYBE_COMMENT_END;
    } else if (c == '/') { /*comment ends */
        state = NORMAL;
    } else if (c == '\n') { /* just in case for corner cases like there 
        is a new line after * */
        state = COMMENT_START;
        putchar('\n');
    }
    else { /* false alarm, comment continues */
        state = COMMENT_START;
    }
    return state;
}
/* Implements the CHAR_START state of the DFA. c is the current
   DFA character. Writes c to stdout. Return the next DFA state. */
handleCharStartState(int c) {
    enum Statetype state;
    if (c == '\\') { /* escape char detected, next char doesn't mean 
        anything */
        state = CHAR_ESCAPE;
    } else if (c == '\'') { /* end of char */
        state = NORMAL;
    }
    else { /* char has not terminated */
        state = CHAR_START;
    }
    putchar(c);
    return state;
}
/* Implements the CHAR_ESCAPE state of the DFA. c is the current
   DFA character. Writes c to stdout. Return the CHAR_START state. */
handleCharEscapeState(int c) { 
    enum Statetype state;
    putchar(c); /* char escape so just put down whatever input and go 
    back to char */
    return CHAR_START;
}
/* Implements the STRING_START state of the DFA. c is the current
   DFA character. Writes c to stdout. Return the next DFA state. */
handleStringStartState(int c) {
    enum Statetype state;
    if (c == '\\') { /* escape char detected, next char doesn't mean 
        anything */
        state = STRING_ESCAPE;
    } else if (c == '\"') { /* end of string */
        state = NORMAL;
    } else { /* string has not terminated */
        state = STRING_START;
    }
    putchar(c);
    return state;
}
/* Implements the STRING_ESCAPE state of the DFA. c is the current
   DFA character. Writes c to stdout. Return the STRING_START state. */
handleStringEscapeState(int c) { 
    enum Statetype state;
    putchar(c); /* char escape so just put down whatever input and 
    go back to string */
    return STRING_START;
}
/* Reads text from stdin. Removes comments denoted by /*...*/ 
/* and replaces it with a space. Write the result to stdout. 
   Returns 1 if there is an unterminated comment and prints an error 
   message with the line number of the unterminated comment to stderr. 
   Returns 0 if there are no unterminated comments. */
int main(void) {
    /* character inputted from stdin*/
    int c;
    /* meant to keep track of line number throughout code running*/
    int line_iterator = 1; 
    /* will update to the start of a comment for error message*/
    int comment_start_line = 1; 
    /* Use a DFA approach. state is the current DFA state. */
    enum Statetype state = NORMAL;
    while ((c = getchar()) != EOF) {
        if (c == '\n') {
            line_iterator++;
        }
        switch (state) {
            case NORMAL:
                state = handleNormalState(c);
                break;
            case MAYBE_COMMENT_START:
                if (c == '*') { /* true only when it is a comment 
                    start */
                    comment_start_line = line_iterator; /* start of 
                    a comment so updates to be the line number of this*/
                }
                state = handleMaybeCommentStartState(c);
                break;
            case COMMENT_START:
                state = handleCommentStartState(c);
                break;
            case MAYBE_COMMENT_END:
                state = handleMaybeCommentEndState(c);
                break;
            case CHAR_START:
                state = handleCharStartState(c);
                break;
            case CHAR_ESCAPE:
                state = handleCharEscapeState(c);
                break;
            case STRING_START:
                state = handleStringStartState(c);
                break;
            case STRING_ESCAPE:
                state = handleStringEscapeState(c);
                break;
        }
    }
    if (state == MAYBE_COMMENT_START) {
        putchar('/'); /* to handle if / is at the end of a line and 
        not a comment*/
    }   
    if (state == COMMENT_START || state == MAYBE_COMMENT_END) {
        fprintf(stderr, "Error: line %d: unterminated comment\n", 
            comment_start_line);
        return 1;
    }
    return 0;
}