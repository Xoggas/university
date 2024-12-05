.386
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.data
  ; 15 байт
  a db 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 0FFh
  
  ; 15 байт
  b db 0FFh, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
  
  ; Для результата резервируется 15 байт
  r db 15 dup(?)

.code
start:
  mov EAX, dword ptr a[0]
  sub EAX, dword ptr b[0]
  mov dword ptr r[0], EAX

  mov EAX, dword ptr a[4]
  sbb EAX, dword ptr b[4]
  mov dword ptr r[4], EAX

  mov EAX, dword ptr a[8]
  sbb EAX, dword ptr b[8]
  mov dword ptr r[8], EAX

  mov AX, word ptr a[12]
  sbb AX, word ptr b[12]
  mov word ptr r[12], AX

  mov AL, byte ptr a[14]
  sbb AL, byte ptr b[14]
  mov byte ptr r[14], AL

  push 0
  call ExitProcess
end start
