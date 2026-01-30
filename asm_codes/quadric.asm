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

; ========= init var a =========

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

; ========= init var b =========

IN

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

; ========= init var c =========

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

; === var c use ===

PUSHREG BX
PUSH 2
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

; === var a use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === func quadric_solver call ===
CALL :quadric_solver

; ===== end call =====

JUMP :end_program

; ======= func discr declaration ========

:discr

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

; ========= init func var par1 =========

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

; ========= init func var par2 =========

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

; ========= init func var par3 =========

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

; ========= init var discr =========

; === var par2 use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === var par2 use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
PUSH 4

; === var par1 use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT

; === var par3 use ===

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

; === var discr use ===

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
PUSHM [AX]
POPREG BX

; ===== area exited =====
RET

; ========= end func declaration ========

; ======= func quadric_solver declaration ========

:quadric_solver

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

; ======== generation if ========

; === var a use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
PUSH 0

PUSH 0
POPREG CX
JNE :cond_end0
PUSH 1
POPREG CX
JUMP :cond_end0
:cond_end0
PUSHREG CX

PUSH 0
JE :endif0

; ======== generation if ========

; === var b use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
PUSH 0

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

; === var c use ===

PUSHREG BX
PUSH 2
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
PUSH 0

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
PUSH 8

OUT
JUMP :endif2


:else2
PUSH 0
PUSH 111
SUB

OUT
JUMP :endif2

:endif2


; ====== end generation if ======
JUMP :endif1


:else1
PUSH 0

; === var c use ===

PUSHREG BX
PUSH 2
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
SUB

; === var b use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
DIV

OUT
JUMP :endif1

:endif1


; ====== end generation if ======
JUMP :endif0

:endif0


; ====== end generation if ======

; ========= init var D =========

; === var c use ===

PUSHREG BX
PUSH 2
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

; === var a use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

; === func discr call ===
CALL :discr

; ===== end call =====

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

; ======== generation if ========

; === var D use ===

PUSHREG BX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
PUSH 0

PUSH 0
POPREG CX
JAE :cond_end3
PUSH 1
POPREG CX
JUMP :cond_end3
:cond_end3
PUSHREG CX

PUSH 0
JE :endif3

PUSH 0
PUSH 111
SUB

OUT
JUMP :endif3

:endif3


; ====== end generation if ======

; ======== generation if ========

; === var D use ===

PUSHREG BX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
PUSH 0

PUSH 0
POPREG CX
JNE :cond_end4
PUSH 1
POPREG CX
JUMP :cond_end4
:cond_end4
PUSHREG CX

PUSH 0
JE :endif4

; ========= init var x =========

PUSH 0

; === var b use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
SUB
PUSH 2

; === var a use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
DIV

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

; === var x use ===

PUSHREG BX
PUSH 4
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT
JUMP :endif4

:endif4


; ====== end generation if ======

; ======== generation if ========

; === var D use ===

PUSHREG BX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
PUSH 0

PUSH 0
POPREG CX
JBE :cond_end5
PUSH 1
POPREG CX
JUMP :cond_end5
:cond_end5
PUSHREG CX

PUSH 0
JE :endif5

; ========= init var x1 =========

PUSH 0

; === var b use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
SUB

; === var D use ===

PUSHREG BX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
SQRT
SUB
PUSH 2

; === var a use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
DIV

PUSHREG BX
PUSH 5
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

; ========= init var x2 =========

PUSH 0

; === var b use ===

PUSHREG BX
PUSH 1
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
SUB

; === var D use ===

PUSHREG BX
PUSH 3
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
SQRT
ADD
PUSH 2

; === var a use ===

PUSHREG BX
PUSH 0
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====
MULT
DIV

PUSHREG BX
PUSH 6
ADD
POPREG FX
POPM [FX]
PUSHM [AX]
PUSH 1
ADD
POPM [AX]

; ========== end init var =========

; === var x1 use ===

PUSHREG BX
PUSH 5
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT

; === var x2 use ===

PUSHREG BX
PUSH 6
ADD
POPREG FX
PUSHM [FX]

; ===== end var =====

OUT
JUMP :endif5

:endif5


; ====== end generation if ======
PUSH 1

; ===== exit area =====
PUSHREG AX

PUSH 1
SUB
POPREG AX
PUSHM [AX]
POPREG BX

; ===== area exited =====
RET

; ========= end func declaration ========
:end_program

HLT
