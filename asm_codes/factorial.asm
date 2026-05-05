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
JUMP :main


; ====== end genetation system zone =====

:main

; ========= init var num =========

IN

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

; ========= init var fact =========

; === var num use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === func factorial call ===
CALL :factorial

; ===== end call =====

PUSHREG BX
PUSH 1
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

; === var fact use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT

JUMP :end_program

; ======= func factorial declaration ========

:factorial

; ======== enter area ======

PUSHM [AX]
POPREG BX
PUSHREG AX
PUSH 1
ADD
POPREG AX
PUSHREG BX
POPM [AX]

; ====== area entered ======

; ========= init func var n =========

PUSHREG BX
PUSH 0
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========= end func var init ========

; ======== generation if ========

; === var n use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
PUSH 1

PUSH 0
POPREG CX
JA :cond_end0
PUSH 1
POPREG CX
JUMP :cond_end0
:cond_end0
PUSHREG CX

PUSH 0
JE :endif0
PUSH 1

; ===== exit area =====
PUSHREG AX

PUSH 1
SUB
POPREG AX
PUSHREG AX
PUSH 1
SUB
POPREG FX
PUSHM [FX]
POPREG BX

; ===== area exited =====
RET
JUMP :endif0

:endif0


; ====== end generation if ======

; === var n use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
PUSH 1
SUB

; === func factorial call ===
CALL :factorial

; ===== end call =====

; === var n use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

; ===== exit area =====
PUSHREG AX

PUSH 1
SUB
POPREG AX
PUSHREG AX
PUSH 1
SUB
POPREG FX
PUSHM [FX]
POPREG BX

; ===== area exited =====
RET

; ========= end func declaration ========
:end_program

HLT
