.386
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.data
  x db -25                        ; знаковое 1-байтовое
  y dd -25                       ; знаковое 4-байтовое
  z dw 100                        ; беззнаковое 2-байтовое
  format db "a = %d", 0         ; форматирующая строка вывода

.code
start:
  movsx eax, x                  ; eax = x, расширяя до 4 байт
  
  mov ecx, y                    ; ecx = y
  add eax, ecx                  ; eax = eax + ecx = x + y

  movsx ecx, z                  ; ecx = z, расширяя до 4 байт
  add eax, ecx                  ; eax = eax + ecx = x + y + z

  mov ecx, eax                  ; ecx = eax = x + y + z
  
  cmp ecx, 0
  jg sum_g_than_zero            ; если x + y + z > 0, то перейти к sum_g_than_zero
  
  mov al, x                     ; al = x

  cmp al, 0                     ; сравнение x с 0
  jg x_g_than_zero              ; если x + y + z <= 0 и x > 0, то перейти к x_g_than_zero
  
  jmp x_le_than_zero            ; если x + y + z <= 0 и x <= 0, то перейти к x_le_than_zero

sum_g_than_zero:
  movsx eax, x                  ; eax = x, расширяя до 4 байт
  movsx ecx, z                  ; ecx = z, расширяя до 4 байт
  imul eax, ecx                 ; eax = eax * ecx = x * z
  mov ecx, eax                  ; ecx = eax = x * z

  movsx eax, x                  ; eax = x, расширяя до 4 байт
  imul eax, eax                 ; eax = eax ^ 2 = x ^ 2
  add eax, 32                   ; eax = eax + 32 = x ^ 2 + 32
  sub eax, ecx                  ; eax = eax - ecx = x ^ 2 + 32 - x * z

  jmp output                    ; переход к выводу результата
  
x_g_than_zero:
  movsx eax, z                  ; eax = z, расширяя до 4 байт
  mov ecx, y                    ; ecx = y
  add eax, ecx                  ; eax = eax + ecx = z + y

  jmp output                    ; переход к выводу результата

x_le_than_zero:
  movsx eax, z                  ; eax = z, расширяя до 4 байт
  mov ecx, 2                    ; ecx = 2 - делитель
  mov edx, 0                    ; обнуление edx перед делением
  div ecx                       ; edx:eax = z / 2 

  mov ecx, y                    ; ecx = y
  imul ecx, eax                 ; ecx = ecx * eax = y * (z / 2)

  movsx eax, x                  ; eax = x, расширяя до 4 байт
  add eax, ecx                  ; eax = eax + ecx = x + y * (z / 2)

  jmp output                    ; переход к выводу результата

output:
  push eax
  push offset format
  call crt_printf

  push 0
  call ExitProcess
end start