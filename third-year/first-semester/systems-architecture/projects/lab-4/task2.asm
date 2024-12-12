.386
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.data
  x dw 500, 600, 700, 800               ; массив знаковых 2-байтовых чисел
  y dd 100, 200, 300, 400               ; массив знаковых 4-байтовых чисел
  m dw 4                                ; беззнаковая 2-байтовая переменная
  n dw 4                                ; беззнаковая 2-байтовая переменная
  format db "a = %d", 0                 ; форматирующая строка вывода
  result dd 0

.code
k proc
  push edi                              ; сохраним значение edi

  mov eax, ebx                          ; eax = i
  cdq                                   ; расширение перед делением
  mov esi, 3                            ; esi = 3
  idiv esi                              ; eax = i / 3
  
  cmp edx, 0                            ; проверка остатка
  je zero_check                         ; если i % 3 = 0, то переходим к zero_check
  jmp result_is_not_multiple            ; иначе переходим к result_is_not_multiple

result_is_not_multiple:
  mov eax, ebx                          ; eax = i
  sub eax, ecx                          ; eax = eax - ecx = i - j
  cmp eax, 0                            ; сравниваем i - j с 0
  jl difference_lower_than_zero         ; если i - j < 0, то перейти к difference_lower_than_zero
  je difference_equals_to_zero          ; если i - j = 0, то перейти к difference_equals_to_zero
  jmp difference_greater_than_zero      ; иначе перейти к difference_greater_than_zero

difference_lower_than_zero:
  neg eax                               ; eax = -eax
  jmp difference_greater_than_zero      ; переход к difference_greater_than_zero

difference_equals_to_zero: 
  mov eax, 1                            ; если i - j = 0, то eax = 1

difference_greater_than_zero:
  mov esi, eax                          ; esi = eax = |i - j|
  mov eax, ebx                          ; eax = i
  add eax, ecx                          ; eax += j
  cdq                                   ; расширение перед делением
  idiv esi                              ; eax /= |i - j|
  mov esi, 5                            ; esi = 5
  imul esi                              ; eax = eax * 5 = ((i + j) / |i - j|) * 5

zero_check:
  cmp eax, 0                            ; проверка результата на 0
  jne return_result                     ; если eax = 0, то перейти к return_result
  mov eax, 1                            ; иначе eax = 1

return_result:
  pop edi                               ; восстанавливаем edi из стека
  ret                                   ; делаем возврат
k endp

start:
  xor esi, esi                          ; обнуление esi
  xor edi, edi                          ; обнуление edi
  xor ebx, ebx                          ; обнуление ebx, в нём будет храниться индекс итерации i
  xor ecx, ecx                          ; обнуление ecx, в нём будет храниться индекс итерации j
  xor edx, edx                          ; обнуление edx, в нём будет храниться сумма
  mov bx, m                             ; bx = m (i)
  dec bx                                ; уменьшаем значение индекса i на единицу
 
loop_i: 
  mov cx, n                             ; cx = n (j)
  dec cx                                ; уменьшаем значение индекса j на единицу
  cmp bx, 0                             ; сравниваем значение регистра bx с 0
  jg loop_j                             ; если значение регистра bx > 0, то перейти к loop_j
  jmp program_end                       ; иначе перейти к output

loop_j:
  mov eax, y[ebx*4]                     ; eax = y[i]
  mov esi, y[ebx*4]                     ; esi = y[i]
  imul esi                              ; eax = eax * esi
  imul esi                              ; eax = eax * esi * esi = y[i]^3

  movsx esi, x[ebx*2]                   ; esi = x[i]
  imul esi                              ; eax = y[i]^3 * x[i]
  imul ebx                              ; eax = y[i]^3 * x[i] * i
  
  mov esi, 6                            ; esi = 6
  add eax, esi                          ; eax = y[i]^3 * x[i] * i + 6
  mov esi, eax                          ; esi = y[i]^3 * x[i] * i + 6

  movsx eax, x[ebx*2]                   ; eax = x[i]
  imul eax                              ; eax = eax * eax = x[i] ^ 2

  xchg eax, esi                         ; свап eax с esi
  cdq                                   ; расширение перед делением
  idiv esi                              ; eax /= esi = (y[i]^3 * x[i] * i + 6) / x[i]^2
  mov edi, eax                          ; edi = (y[i]^3 * x[i] * i + 6) / x[i]^2

  call k                                ; eax = k(i, j)
  mov esi, eax                          ; esi = k(i, j)
  movsx eax, x[ebx*2]                   ; eax = x[i]
  cdq                                   ; расширение перед делением
  idiv esi                              ; eax /= esi = x[i] / k(i, j)
  sub eax, edi                          ; eax -= edi = x[i] / k(i, j) - (y[i]^3 * x[i] * i + 6) / x[i]^2

  add result, eax                       ; добавляем результат в общую сумму

  loop loop_j                           ; если cx (j) != 0, то перейти к loop_j
  dec bx                                ; после завершения цикла j уменьшаем bx (i)
  jmp loop_i                            ; переходим в цикл i

program_end:
  push result                           ; помещем вычисленную сумму в стек
  push offset format                    ; помещаем адрес форматирующей строки в стек
  call crt_printf                       ; вызываем функцию вывода

  push 0                                ; помещаем exit код в стек
  call ExitProcess                      ; завершаем выполнение программы
end start