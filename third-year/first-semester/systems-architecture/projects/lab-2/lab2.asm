.686
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.code
start:
  MOV EAX, 7800h
  DB 0B8h, 078h, 00h  
  push 0                        ; Поместить аргумент функции в стек
  call ExitProcess              ; Вызов функции ExitProcess(0)
END start