JUMP :main


; ============== ФУНКЦИИ ===============

;Название функции
:factorial

;Инцилизация параматра n функции 
PUSH 0
POPREG AX
POPM [AX]
PUSH 0
POPREG AX
PUSHM [AX]
PUSH 0
PUSH 0
POPREG CX
JNE :false1
PUSH 1
POPREG CX
:false1
PUSHREG CX

;Условный оператор
PUSH 0
JE :endif1
PUSH 1
RET
:endif1
Условного оператора


;Инцилизация переменной prev
PUSH 1
POPREG AX
PUSH 1
PUSH 0
POPREG AX
PUSHM [AX]

;вызов функции
CALL :factorial
POPM [AX]

PUSH 0
POPREG AX
PUSHM [AX]
PUSH 1
POPREG AX
PUSHM [AX]
MULT
RET

; ============== КОНЕЦ ФУНКЦИЙ ============



; ============ ГЛАВНАЯ ПРОГРАММА ============
:main

;Инцилизация переменной x
PUSH 0
POPREG AX
PUSH 5

;вызов функции
CALL :factorial
POPM [AX]


; ================ КОНЕЦ ПРОГРАММЫ ================

HLT
