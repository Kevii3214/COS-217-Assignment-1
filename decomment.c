/*--------------------------------------------------------------------*/
/* decomment.c                                                           */
/* Author: Kevin Tran                                               */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>

enum Statetype {NORMAL, MAYBE_COMMENT_START, COMMENT_START,MAYBE_COMMENT_END, CHAR_START, CHAR_ESCAPE, STRING_START, STRING_ESCAPE};
enum Statetype
handleNormalState(int c) {
    enum Statetype state;
    if (c== '/') {
        /* replace char with nothing, must see if it's a comment first */
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
enum Statetype
handleMaybeCommentStartState(int c) {
    enum Statetype state;
    if (c == '/') { /* should put down another / since one is missing from earlier */
        state = MAYBE_COMMENT_START;
        putchar(c);
    } else if (c == '*') {
    /* comment marker found, replace with a space */
        state = COMMENT_START;
        putchar(' ');
    } else if (c == '\'') {
        state = CHAR_START;
        putchar(c);
    } else if (c == '\"') {
        state = STRING_START;
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
enum Statetype
handleCommentStartState(int c) {
    enum Statetype state;
    if (c == '*') { /* remove just in case it is the end of comment */
        state = MAYBE_COMMENT_END;
    } else { /* still in comment, keep putting down characters */
        state = COMMENT_START;
    }
    return state;
}
enum Statetype
handleMaybeCommentEndState(int c) {
    enum Statetype state;
    if (c == '*') { /* still possible to terminate comment, put down * since we removed one earlier */
        state = MAYBE_COMMENT_END;
    } else if (c == '/') { /*comment ends */
        state = NORMAL;
    } else { /* false alarm, comment continues */
        state = COMMENT_START;
    }
    return state;
}
enum Statetype
handleCharStartState(int c) {
    enum Statetype state;
    if (c == '\\') { /* escape char detected, next char doesn't mean anything */
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
enum Statetype
handleCharEscapeState(int c) { 
    enum Statetype state;
    state = CHAR_START;
    putchar(c); /* char escape so just put down whatever input and go back to char */
    return state;
}
enum Statetype
handleStringStartState(int c) {
    enum Statetype state;
    if (c == '\\') { /* escape char detected, next char doesn't mean anything */
        state = STRING_ESCAPE;
    } else if (c == '\"') { /* end of string */
        state = NORMAL;
    } else { /* string has not terminated */
        state = STRING_START;
    }
    putchar(c);
    return state;
}
enum Statetype
handleStringEscapeState(int c) { 
    enum Statetype state;
    state = STRING_START;
    putchar(c); /* char escape so just put down whatever input and go back to string */
    return state;
}

int main(void) {
    int c;
    enum Statetype state = NORMAL;
    while ((c = getchar()) != EOF) {
        switch (state) {
            case NORMAL:
                state = handleNormalState(c);
                break;
            case MAYBE_COMMENT_START:
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
    if (state == NORMAL ||state == MAYBE_COMMENT_START || state == CHAR_START || state == CHAR_ESCAPE || state == STRING_START || state == STRING_ESCAPE) {
        return 0;
    }
    return 1;
}