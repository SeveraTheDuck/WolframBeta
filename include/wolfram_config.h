#ifndef WOLFRAM_CONFIG_H
#define WOLFRAM_CONFIG_H

enum data_type
{
    NO_TYPE   = 0,
    NUMBER    = 1,
    OPERATION = 2,
    VARIABLE  = 3,
};

enum operation
{
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    SIN = 4,
};

#endif /* WOLFRAM_CONFIG_H */
