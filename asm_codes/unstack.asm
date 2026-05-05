; ===== generation system memory zone =====

PUSH 0
POPREG AX

PUSH 100
POPM [AX]

PUSH 1
POPREG AX

PUSH 100
POPM [AX]

PUSHM [AX]
POPREG BX

PUSH 100
POPREG HX
JUMP :main


; ====== end genetation system zone =====

:main

; ========= init var a =========
PUSH 2

PUSHREG BX
PUSH 0
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

PUSH 0

; === var a use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
SUB

PUSHREG HX
PUSH 0
ADD
POPREG FX
POPM [FX]

; === var a use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT

PUSH 0

; === var a use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
SUB

OUT

JUMP :end_program
:end_program

HLT
