#ifndef WOLFRAM_CONFIG_H
#define WOLFRAM_CONFIG_H

enum data_type
{
    NO_TYPE   = 0x0,
    NUMBER    = 0x1,
    OPERATION = 0x2,
    VARIABLE  = 0x3,
};

enum binary_operation
{
    ADD = 0,
    SUB,
    MUL,
    DIV,
    POW,

    NUM_OF_BIN_OP
};

enum unary_operation
{
    SIN = NUM_OF_BIN_OP,

    NUM_OF_UN_OP = 1
};

const size_t NUM_OF_OP = NUM_OF_BIN_OP + NUM_OF_UN_OP;

const char* const operations_array [NUM_OF_OP] =
    {"+", "-", "*", "/", "^", "SIN"};

#endif /* WOLFRAM_CONFIG_H */
