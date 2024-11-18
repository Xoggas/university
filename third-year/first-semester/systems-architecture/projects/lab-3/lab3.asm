.386
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.data
  x dw -10
  y dw 5
  z dw 50
  format_string db "x = %hd, y = %hd, z = %hd, remainder = %d, R = %d", 0

.code
start:
  ; Вычисление (x + 10)
  movsx eax, x        ; Расширение x до двойного слова
  add eax, 10         ; EAX = x + 10
  mov ebx, eax        ; EBX = x + 10

  ; Вычисление (y - 5)
  movsx eax, y        ; Расширение y до двойного слова
  sub eax, 5          ; EAX = y - 5
  imul ebx, eax       ; EBX = (x + 10) * (y - 5)

  ; Вычисление (z / 3) и остатка
  movsx eax, z        ; Расширение z до двойного слова
  cdq                 ; Расширение знака в EDX:EAX
  mov ecx, 3          ; Делитель = 3
  idiv ecx            ; EAX = z / 3, EDX = z % 3 (остаток)
  mov ecx, eax        ; ECX = z / 3 (частное)

  ; Вычисление (z - z / 3)
  movsx eax, z        ; Расширение z до двойного слова
  sub eax, ecx        ; EAX = z - z / 3
  imul ebx, eax       ; EBX = (x + 10) * (y - 5) * (z - z / 3)

  ; Вычитание 7^4
  mov eax, 7*7*7*7    ; EAX = 7^4
  sub ebx, eax        ; EBX = (x + 10) * (y - 5) * (z - z / 3) - 7^4
  mov eax, ebx        ; EAX = результат

  ; Вывод результатов
  push eax            ; R
  push edx            ; Остаток от деления (z / 3)
  push dword ptr z    ; z
  push dword ptr y    ; y
  push dword ptr x    ; x
  push offset format_string
  call crt_printf
  add esp, 6*4        ; Освобождаем стек

  ; Завершение программы
  push 0
  call ExitProcess
end start
