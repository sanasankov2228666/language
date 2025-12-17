IN;         R
POPREG CX

IN; разрешение по горизонтали   X
POPREG DX

IN; разрешение по вертикали     Y
POPREG EX

IN;         горизонталь
POPREG AX;  X

IN;         вертикаль
POPREG BX;  y 

CALL :draw_circle

CALL :END

:draw_circle
    PUSH 0
    POPREG FX
    
    CALL :inequality
    RET

:inequality
    PUSHREG DX
    PUSHREG FX
    DIV
    POPREG HX;  y

    PUSHREG DX
    PUSHREG HX
    MULT
    PUSHREG FX
    SUB
    POPREG GX; x

    PUSHREG GX
    PUSHREG AX
    SUB
    PUSHREG GX
    PUSHREG AX
    SUB
    MULT

    PUSHREG HX
    PUSHREG BX
    SUB
    PUSHREG HX
    PUSHREG BX
    SUB
    MULT

    ADD

    PUSHREG CX
    PUSHREG CX
    MULT

    JB :ONE

    PUSHREG GX
    PUSHREG AX
    SUB
    PUSHREG GX
    PUSHREG AX
    SUB
    MULT

    PUSHREG HX
    PUSHREG BX
    SUB
    PUSHREG HX
    PUSHREG BX
    SUB
    MULT

    ADD

    PUSHREG CX

    JA :ZERO

    RET

:ONE
    PUSH 35
    POPM [FX]

    PUSH 1
    PUSHREG FX
    ADD
    POPREG FX

    PUSHREG FX
    PUSHREG DX
    PUSHREG EX
    MULT

    JB :inequality
    JUMP :END

:ZERO
    PUSH 46
    POPM [FX]

    PUSH 1
    PUSHREG FX
    ADD
    POPREG FX

    PUSHREG FX
    PUSHREG DX
    PUSHREG EX
    MULT

    JB :inequality
    JUMP :END

:END
    DRAW
    HLT