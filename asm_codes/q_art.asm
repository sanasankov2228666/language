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

; ========= init var two =========
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

; ========= init var zero =========
PUSH 0

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

; ========= init var A =========

IN

PUSHREG BX
PUSH 2
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

; ========= init var B =========

IN

PUSHREG BX
PUSH 3
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

; ========= init var C =========

IN

PUSHREG BX
PUSH 4
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

; ======== generation if ========

; === var A use ===

PUSHREG HX
PUSH 2
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var zero use ===

PUSHREG HX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

PUSH 0
POPREG CX
JNE :cond_end0
PUSH 1
POPREG CX
JUMP :cond_end0
:cond_end0
PUSHREG CX

PUSH 0
JE :else0

; ======== generation if ========

; === var B use ===

PUSHREG HX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var zero use ===

PUSHREG HX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

PUSH 0
POPREG CX
JNE :cond_end1
PUSH 1
POPREG CX
JUMP :cond_end1
:cond_end1
PUSHREG CX

PUSH 0
JE :else1

; ======== generation if ========

; === var C use ===

PUSHREG HX
PUSH 4
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var zero use ===

PUSHREG HX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

PUSH 0
POPREG CX
JNE :cond_end2
PUSH 1
POPREG CX
JUMP :cond_end2
:cond_end2
PUSHREG CX

PUSH 0
JE :else2

; ========= init var infinity =========

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

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

; === var infinity use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT
JUMP :endif2


:else2
; ========= init var Unluck =========

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
SUB

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
DIV
ADD

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

; === var Unluck use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT
JUMP :endif2

:endif2


; ====== end generation if ======
JUMP :endif1


:else1
; ========= init var X =========

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
SUB

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
DIV
ADD

; === var C use ===

PUSHREG HX
PUSH 4
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

; === var B use ===

PUSHREG HX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
DIV

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

; === var X use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT
JUMP :endif1

:endif1


; ====== end generation if ======
JUMP :endif0


:else0
; ========= init var Discr =========

; === var C use ===

PUSHREG HX
PUSH 4
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var B use ===

PUSHREG HX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var A use ===

PUSHREG HX
PUSH 2
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === func Dis_cr_func call ===
CALL :Dis_cr_func

; ===== end call =====

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

; ======== generation if ========

; === var Discr use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var zero use ===

PUSHREG HX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

PUSH 0
POPREG CX
JAE :cond_end3
PUSH 1
POPREG CX
JUMP :cond_end3
:cond_end3
PUSHREG CX

PUSH 0
JE :else3

; ========= init var unluck =========

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
SUB

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
DIV
ADD

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

; === var unluck use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT
JUMP :endif3


:else3
; ========= init var discr =========

; === var Discr use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
SQRT

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

; ======== generation if ========

; === var Discr use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var zero use ===

PUSHREG HX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

PUSH 0
POPREG CX
JNE :cond_end4
PUSH 1
POPREG CX
JUMP :cond_end4
:cond_end4
PUSHREG CX

PUSH 0
JE :else4

; ========= init var X1 =========

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
SUB

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
DIV
ADD

; === var B use ===

PUSHREG HX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

; === var discr use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
ADD

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var A use ===

PUSHREG HX
PUSH 2
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
DIV

PUSHREG BX
PUSH 2
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

; === var X1 use ===

PUSHREG BX
PUSH 2
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT
JUMP :endif4


:else4
; ========= init var X1 =========

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
SUB

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
DIV
ADD

; === var B use ===

PUSHREG HX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

; === var discr use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
ADD

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var A use ===

PUSHREG HX
PUSH 2
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
DIV

PUSHREG BX
PUSH 2
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

; === var X1 use ===

PUSHREG BX
PUSH 2
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT

; ========= init var X2 =========

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
SUB

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
DIV
ADD

; === var B use ===

PUSHREG HX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

; === var discr use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
SUB

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var A use ===

PUSHREG HX
PUSH 2
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
DIV

PUSHREG BX
PUSH 3
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

; === var X2 use ===

PUSHREG BX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT
JUMP :endif4

:endif4


; ====== end generation if ======
JUMP :endif3

:endif3


; ====== end generation if ======
JUMP :endif0

:endif0


; ====== end generation if ======

JUMP :end_program

; ======= func Dis_cr_func declaration ========

:Dis_cr_func

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

; ========= init func var a =========

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

; ========= init func var b =========

PUSHREG BX
PUSH 1
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========= end func var init ========

; ========= init func var c =========

PUSHREG BX
PUSH 2
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========= end func var init ========

; ========= init var D =========

; === var b use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var b use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var two use ===

PUSHREG HX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

; === var a use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

; === var c use ===

PUSHREG BX
PUSH 2
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
SUB

PUSHREG BX
PUSH 3
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

; === var D use ===

PUSHREG BX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

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
