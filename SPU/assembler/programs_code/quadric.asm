IN
IN
IN
POPREG CX
POPREG BX
POPREG AX

PUSH 0
PUSHREG AX
JE :line_сase;  проверка на линейность уравнения

CALL :discr

PUSHREG DX
PUSH 0
JB :end_zero

PUSHREG AX
PUSH 2
MULT

PUSHREG DX
SQRT

PUSHREG BX
PUSH 0
SUB

ADD
DIV
POPREG EX


PUSHREG DX
PUSH 0
JB :end_zero

PUSHREG AX
PUSH 2
MULT

PUSHREG DX
SQRT

PUSHREG BX
PUSH 0
SUB

SUB
DIV
POPREG FX

JUMP :end_two

:discr

    PUSHREG AX
    PUSHREG CX
    PUSH 4
    MULT
    MULT

    PUSHREG BX
    PUSHREG BX
    MULT

    SUB
    POPREG DX
    RET

:line_сase

    PUSHREG BX
    PUSHREG 0
    JE :inf_case

    PUSHREG BX
    PUSHREG CX
    PUSH 0
    SUB
    DIV
    POPREG EX
    JUMP :end_one


:inf_case

    PUSHREG CX
    PUSH 0
    JA :end_inf
    JUMP :end_zero

:end_inf

    PUSH 8
    OUT
    HLT

:end_zero

    PUSH -1
    OUT
    HLT

:end_one

    PUSHREG EX
    OUT
    HLT

:end_two
    PUSHREG EX
    PUSHREG FX
    OUT
    OUT
    HLT