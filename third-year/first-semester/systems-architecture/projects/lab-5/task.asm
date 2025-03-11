.386
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.data
  fmt_string db "S = %lf, n = %lf", 10, 0  ; форматирующая строка
  x_lower_than_zero_flag db 0              ; флаг отрицательности x
  y_lower_than_zero_flag db 0              ; флаг отрицательности y
  power dq 2.0                             ; показатель степени
  iterator dd 1000                         ; количество итераций

.code
rem_stack_elements proc
  ffree st(0)                         ; st(0) = 0
  fincstp                             ; убираем элемент из стека
  ret                                 ; выходим из функции
rem_stack_elements endp           
           
pow proc           
  mov x_lower_than_zero_flag, 0       ; сбрасываем флаг отрицательности x
  mov y_lower_than_zero_flag, 0       ; сбрасываем флаг отрицательности y
  fld qword ptr [esp+12]              ; st(1) = y
  fldz                                ; загружаем 0 в стек для сравнения
  db 0dfh, 0f0h+1                     ; проверяем, если y == 0
  je y_equals_zero                    ; если y == 0, то перейти к y_equals_zero
  ja y_lower_than_zero                ; если y < 0, то y_lower_than_zero
  jmp init_x                          ; если y > 0, то перейти к init_x
             
y_lower_than_zero:  
  fchs                                ; abs(y)
  mov y_lower_than_zero_flag, 1       ; устанавливаем флаг отрицательности y
  jmp init_x                          ; переходим к init_x
           
init_x:           
  fld qword ptr [esp+4]               ; st(0) = x
  fldz                                ; загружаем 0 в стек для сравнения
  db 0dfh, 0f0h+1                     ; проверяем, если x == 0
  je x_equals_zero                    ; если x == 0, то перейти к x_equals_zero
  ja x_lower_than_zero                ; если x < 0, то перейти к x_lower_than_zero
  jmp main                            ; если x > 0, то перейти к main
             
x_lower_than_zero:           
  fchs                                ; abs(x)
  mov x_lower_than_zero_flag, 1       ; устанавливаем флаг отрицательности x
  jmp main                            ; переходим к main
           
main:           
  fyl2x                               ; Вычисляем y * log2(x)
  sub esp, 8                          ; выделяем 8 байт для y * log2(x)
  fstp qword ptr [esp]                ; записываем y * log2(x) в стек
  fld1                                ; st(1) = 1
  fld qword ptr [esp]                 ; st(0) = y * log2(x)
  fprem                               ; st(0) = (y * log2(x)) % 1
  sub esp, 8                          ; выделяем место на стеке пол дробную часть
  fst qword ptr [esp]                 ; записываем дробную часть в стек
  fld qword ptr [esp+8]               ; st(0) = y * log2(x)
  fsub st(0), st(1)                   ; st(0) = truncate(y * log2(x))
  sub esp, 8                          ; выделяем место для целой части
  fstp qword ptr [esp]                ; записываем целую часть в стек
  call rem_stack_elements             ; убираем два лишних элемента из стека
  call rem_stack_elements             
  fld qword ptr [esp]                 ; st(1) = a
  fld1                                ; st(0) = 1
  fscale                              ; st(0) = 2^a
  sub esp, 8                          ; выделяем место 2^a
  fstp qword ptr [esp]                ; записываем целую часть в стек
  call rem_stack_elements             ; убираем два лишних элемента из стека
  fld qword ptr [esp+16]              ; st(0) = b
  f2xm1                               ; st(0) = 2^b-1
  fld1                                ; st(1) = 2^b-1, st(0) = 1
  fadd st(0), st(1)                   ; st(0) = 2^b
  fld qword ptr [esp]                 ; st(1) = 2^b, st(0) = 2^a
  fmul st(0), st(1)                   ; st(0) = 2^a * 2^b
  add esp, 32                         ; очищаем стек от временных элементов
  jmp y_was_negative_check            ; переходим к y_was_negative_check
  
y_was_negative_check:
  movzx eax, y_lower_than_zero_flag   ; проверяем флаг отрицательности y
  mov ebx, 1                          ; ожидаемое значение флага
  cmp eax, ebx                        ; сравнение значений y_lower_than_zero_flag
  je y_was_negative                   ; y < 0, то перейти к y_was_negative
  jmp x_was_negative_check            ; иначе перейти к x_was_negative_check

x_was_negative_check:                  
  movzx eax, x_lower_than_zero_flag   ; проверяем флаг отрицательности x
  mov ebx, 1                          ; ожидаемое значение флага
  cmp eax, ebx                        ; сравнение значений x_lower_than_zero_flag
  je x_was_negative                   ; x < 0, то перейти к x_was_negative
  jmp return                          ; иначе перейти к return
  
y_was_negative:
  fld1                                ; загружаем 1 в st(0)
  fdiv st(0), st(1)                   ; st(0) = 1/x^y
  sub esp, 8                          ; выделяем место на стеке
  fstp qword ptr [esp]                ; сохраняем результат деления в стек   
  call rem_stack_elements             ; удаляем делимое из стека
  fld qword ptr [esp]                 ; загружаем результат из стека 
  add esp, 8                          ; возвращаем выделенную память 
  jmp x_was_negative_check            ; переходим к проверке отрицательности x

x_was_negative:
  fchs                                ; если x < 0, то меняем знак получившегося результата
  jmp return                          ; переходим к return

return:
  ret 16                              ; выполняем возврат, очищая стек аргументов
           
y_equals_zero:           
  call rem_stack_elements             ; убираем y из стека
  fld1                                ; st(0) = 1
  ret 16                              ; возвращаемся обратно
           
x_equals_zero:           
  call rem_stack_elements             ; убираем x и y из стека
  call rem_stack_elements           
  fldz                                ; st(0) = 0
  ret 16                              ; возвращаемся обратно
pow endp           
           
start:    
  finit                               ; инициализируем FPU

  sub esp, 8                          ; выделяем место на стеке для хранения индекса итерации
  fld1                                ; st(0) = n = 1
  fstp qword ptr [esp]                ; помещаем индекс итерации в стек
  
  sub esp, 8                          ; выделяем место на стеке для хранения суммы
  fldz                                ; st(0) = S = 0
  fstp qword ptr [esp]                ; помещаем сумму в стек

sum_loop:   
  fld1                                ; st(1) = 1
  fld qword ptr [esp+8]               ; st(0) = n
  fpatan                              ; st(0) = arctg(st(1)/st(0)) = arctg(1/n)
  sub esp, 8                          ; выделяем место на стеке для arctg(1/n)
  fstp qword ptr [esp]                ; записываем результат на стек

  fld qword ptr [esp+16]              ; st(1) = n
  fld1                                ; st(0) = 1
  fsubp st(1), st                     ; st(0) = n - 1
  sub esp, 8                          ; выделяем место на стеке для n - 1
  fstp qword ptr [esp]                ; записываем результат на стек

  push dword ptr power[4]             ; загружаем показатель степени в стек
  push dword ptr power[0]    
  push dword ptr [esp+36]             ; загружаем значение n в стек    
  push dword ptr [esp+36]             
  call pow                            ; вызываем функцию возвдения в степень
  sub esp, 8                          ; выделяем место на стеке для n^2
  fstp qword ptr [esp]                ; записываем n^2 на стек
  call rem_stack_elements             ; удаляем из стека лишние элементы после вызова функции pow
  call rem_stack_elements

  fld qword ptr [esp+8]               ; st(1) = n-1
  fld qword ptr [esp]                 ; st(0) = n^2
  fdivp st(1), st                     ; st(0) = (n-1)/n^2
  add esp, 16                         ; возвращаем указатель стека к atan(1/n)
  fld qword ptr [esp]                 ; st(0) = atan(1/n)
  faddp st(1), st                     ; st(0) = atan(1/n) + (n-1)/n^2
  
  add esp, 8                          ; переводим указатель стека на S
  fld qword ptr [esp]                 ; загружаем сумму в стек FPU
  faddp st(1), st                     ; складываем получившиеся числа
  fstp qword ptr [esp]                ; выгружаем сумму обратно в стек

  push offset fmt_string              ; загружаем указатель на форматирующую строку в стек
  call crt_printf                     ; выводим сумму и номер итерации 
  add esp, 4                          ; очищаем стек от указателя на форматирующую строку

  fld qword ptr [esp+8]               ; st(1) = n
  fld1                                ; st(0) = 1
  faddp st(1), st                     ; st(0) = n + 1
  fstp qword ptr [esp+8]              ; сохраняем n в стек
  call rem_stack_elements             ; удаляем индекс итерации со стека
  
  dec iterator
  mov eax, 0
  cmp iterator, eax
  jne sum_loop                        ; следующая итерация цикл
           
  push 0                              ; помещаем exit code в стек
  call ExitProcess                    ; выходим из программы
end start