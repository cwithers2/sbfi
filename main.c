#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CHUNK_MEMORY 1024

typedef struct Chunk Chunk;
typedef struct Tape Tape;
typedef struct Loop Loop;

enum Error{
    ERROR_MEM_ALLOC = 1, /* Malloc/calloc error. */
    ERROR_DEAD_FILE = 2, /* I/O error with a file. */
    ERROR_WRONG_USE = 3, /* No file was given to read. */
    ERROR_FILE_OPEN = 4, /* Could not open the program file. */
    ERROR_LOOP_JUMP = 5, /* No closing bracket found. */
    ERROR_LOOP_BACK = 6  /* Unexpected closing bracket. */
};

enum Token{
    SHIFT_RIGHT = '>',
    SHIFT_LEFT  = '<',
    PLUS        = '+',
    MINUS       = '-',
    OUTPUT      = '.',
    INPUT       = ',',
    LOOP_JUMP   = '[',
    LOOP_BACK   = ']'
};

struct Loop{ /* stack node */
    Loop* prev;
    long int pos;
};

struct Chunk{ /* double linked list node */
    Chunk* next;
    Chunk* prev;
    char   memory[CHUNK_MEMORY];
};

struct Tape{
    Chunk  head; /* head of the linked list */
    Chunk* curr; /* current Chunk the ptr is in */
    char*  ptr;  /* data pointer */
};

void init(Tape*);
void release(Tape*);

int shift_left(Tape*);
int shift_right(Tape*);

int loop_jump(Tape*, FILE*, Loop**);
int loop_back(Tape*, FILE*, Loop**);

int output(Tape*);
int input(Tape*);

int interpret(FILE*);

void init(Tape* tape){
    tape->head.next = tape->head.prev = NULL;
    tape->curr = &(tape->head);
    tape->ptr = tape->curr->memory;

    memset(tape->ptr, 0, CHUNK_MEMORY);
}

void release(Tape* tape){
    Chunk* c;
    for(c = tape->head.next; c; c = c->next)
        free(c);
    for(c = tape->head.prev; c; c = c->prev)
        free(c);
}

int shift_left(Tape* tape){
    if(tape->ptr == tape->curr->memory){
        if(tape->curr->prev == NULL){
            Chunk* temp = (Chunk*)calloc(1, sizeof(Chunk));
            if(!temp)
                return ERROR_MEM_ALLOC;
            tape->curr->prev = temp;
            temp->next = tape->curr;
            temp->prev = NULL;
            tape->curr = temp;
        }else{
            tape->curr = tape->curr->prev;
        }
        tape->ptr = tape->curr->memory + (CHUNK_MEMORY-1);
    }else{
        tape->ptr -= 1;
    }

    return 0;
}

int shift_right(Tape* tape){
    if(tape->ptr == (tape->curr->memory)+CHUNK_MEMORY-1){
        if(tape->curr->next == NULL){
            Chunk* temp = (Chunk*)calloc(1, sizeof(Chunk));
            if(!temp)
                return ERROR_MEM_ALLOC;
            tape->curr->next = temp;
            temp->prev = tape->curr;
            temp->next = NULL;
            tape->curr = temp;
        }else{
            tape->curr = tape->curr->next;
        }
        tape->ptr = tape->curr->memory;
    }else{
        tape->ptr += 1;
    }
    return 0;
}

int loop_jump(Tape* tape, FILE* file, Loop** stack){
    unsigned int depth;
    int token;
    if(!*(tape->ptr)){
        depth = 1;
        while(depth && (token = getc(file)) != EOF){
            switch(token){
                case LOOP_JUMP:
                    ++depth;
                    break;
                case LOOP_BACK:
                    --depth;
                    break;
                default:
                    break;
            }
        }
        if(ferror(file))
            return ERROR_DEAD_FILE;
        if(token == EOF)
            return ERROR_LOOP_JUMP;
    }else{
        Loop *node = malloc(sizeof(Loop));
        if(!node)
            return ERROR_MEM_ALLOC;
        node->pos = ftell(file);
        if(node->pos == -1)
            return ERROR_DEAD_FILE;
        node->prev = *stack;
        *stack = node;
    }
    return 0;
}

int loop_back(Tape* tape, FILE* file, Loop** stack){
    Loop* popped;

    if(!*stack)
        return ERROR_LOOP_BACK;

    if(*(tape->ptr)){
        if(fseek(file, (*stack)->pos, SEEK_SET) != 0)
            return ERROR_DEAD_FILE;
    }else{
        popped = *stack;
        *stack = (*stack)->prev;
        free(popped);
    }
    return 0;
}

int output(Tape* tape){
    int token = putchar(*(tape->ptr));
    if(token == EOF)
        return ERROR_DEAD_FILE;
    return 0;
}

int input(Tape* tape){
    int token = getc(stdin);
    if(token == EOF)
        return ERROR_DEAD_FILE;
    else
        *(tape->ptr) = token;
    return 0;
}

int interpret(FILE* file){
    int token;
    int status;
    Tape tape;
    Loop* stack;

    init(&tape);

    while(!status && (token = getc(file)) != EOF){
        switch(token){
            case(SHIFT_RIGHT):
                status = shift_right(&tape);
                break;
            case(SHIFT_LEFT):
                status = shift_left(&tape);
                break;
            case(PLUS):
                ++*(tape.ptr);
                break;
            case(MINUS):
                --*(tape.ptr);
                break;
            case(OUTPUT):
                status = output(&tape);
                break;
            case(INPUT):
                status = input(&tape);
                break;
            case(LOOP_JUMP):
                status = loop_jump(&tape, file, &stack);
                break;
            case(LOOP_BACK):
                status = loop_back(&tape, file, &stack);
                break;
            default:
                break;
        }
    }
    if(ferror(file))
        status = ERROR_DEAD_FILE;

    release(&tape);

    return status;
}

int main(int argc, char** argv){
    int status;
    FILE* file;

    if(argc < 2){
        status = ERROR_WRONG_USE;
    }else{
        file = fopen(argv[1], "r");

        if(!file){
            status = ERROR_FILE_OPEN;
        }else{
            status = interpret(file);
            fclose(file);
        }
    }
    return status;
}
