.686
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
include user32.inc
includelib kernel32.lib
includelib msvcrt.lib
includelib user32.lib

.data
    a DD 30201, 30201h
    b DB 43h, 0F3h, 0F3h, 0E5h
      DF 1500
      DD 1.5, 1.6, 1.9, -1.9
    t DQ 0E7D32A1h
    stra DB 16 dup(1)

    str_fmt DB 6 dup("%d", 9, "%p", 9, "%p", 9, "%s", 10), 13, 0
    plain_fmt_str DB "%s", 13, 0
    a_name DB "a", 0
    b_name DB "b", 0
    undefined DB "-", 0
    t_name DB "t", 0
    stra_name DB "stra", 0
    window_name DB "Output", 0
    buffer DB 74 dup(0)

.code
start:
    push offset stra_name
    push offset stra
    push offset stra + 16 - 1
    push 16

    push offset t_name
    push offset t
    push offset t + 8 - 1
    push 8

    push offset undefined
    push offset b + 4 + 6
    push offset b + 4 + 6 + 16 - 1
    push 16

    push offset undefined
    push offset b + 4
    push offset b + 4 + 6 - 1
    push 6

    push offset b_name
    push offset b
    push offset b + 4 - 1
    push 4

    push offset a_name
    push offset a
    push offset a + (offset b - offset a) - 1
    push offset b - offset a 
    
    push offset str_fmt
    push offset buffer
    call crt_sprintf
    add esp, 6 * 4 + 2

    push offset buffer 
    call crt_puts
    add esp, 4

    push 0
    push offset window_name
    push offset buffer
    push 0
    call MessageBoxA

    push 0                        
    call ExitProcess
end start