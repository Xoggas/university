.686
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.data
    a DD 30201, 30201h            ; массив из двух двойных слов
    b DB 43h, 0F3h, 0F3h, 0E5h    ; массив из четырёх слов
      DF 1500                     ; шестибайтовое число без имени 
      DD 1.5, 1.6, 1.9, -1.9      ; массив из четырёх двойных слов без имени
    t DQ 0E7D32A1h                ; восьмибайтовое слово 
    stra DB 16 dup(1)             ; массив слов с 16-ю единичками (1, 1, 1, 1, ...)

.code
start:
    MOV ESI, 65737341h            ; ESI = 65737341h
    AND ESI, dword ptr b          ; ESI = ESI & *b
    MOV dword ptr stra, ESI       ; stra[0-3] = ESI[0-3] 
    MOV ECX, dword ptr t          ; ECX = *t
    IMUL ECX, 7                   ; ECX = ECX * 7
    ADD ECX, 6                    ; ECX = ECX + 6
    MOV dword ptr stra[4], ECX    ; stra[4-7] = ECX[0-3]
    ADD stra[8], 'q'              ; stra[8] += 'q'
    DEC stra[9]                   ; stra[9] -= 1

    push offset stra
    call crt_puts                 ; puts(stra)
    ADD ESP, 4                    ; Очистка стека от аргумента

    call crt__getch               ; Задержка ввода, getch() 
    push 0                        ; Поместить аргумент функции в стек
    call ExitProcess              ; Вызов функции ExitProcess(0)
END start