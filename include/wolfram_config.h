#ifndef WOLFRAM_CONFIG_H
#define WOLFRAM_CONFIG_H

enum data_type
{
    NO_TYPE = 0,
    NUMBER,
    OPERATION,
    VARIABLE
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
    COS,

    NUM_OF_UN_OP = 2
};

const size_t NUM_OF_OP = NUM_OF_BIN_OP + NUM_OF_UN_OP;

const char* const operations_array [NUM_OF_OP] =
    {"+", "-", "*", "/", "^", "SIN", "COS"};

#endif /* WOLFRAM_CONFIG_H */
