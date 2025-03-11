.386
.model flat, stdcall
option casemap: none

include windows.inc
include kernel32.inc
include msvcrt.inc
includelib kernel32.lib
includelib msvcrt.lib

.data
    num_of_bytes dd 36
    source db 56h, 34h, 12h, 90h, 0EFh, 0CDh, 0ABh, 90h, 78h, 56h, 34h, 12h, 0EFh, 0CDh, 0ABh, 90h, 78h, 56h, 34h, 12h, 0EFh, 0CDh, 0ABh, 90h, 78h, 56h, 34h, 12h, 0EFh, 0CDh, 0ABh, 90h, 0, 0, 0
    result db 36 dup(0)
    power dd 8
    fmt_string db "%02X", 0
    new_line_string db 13, 10, 0

.code
divide proc
    mov eax, [esp+4]                         ; eax = *source
    mov ebx, [esp+12]                        ; ebx = *result
    mov ecx, num_of_bytes                    ; ecx = num_of_bytes

    copy_cycle:                     
    mov dl, [eax+ecx-1]                      ; копируем байт из *(source + ecx)
    mov [ebx+ecx-1], dl                      ; вставляем байт в *(result + ecx)
    loop copy_cycle

    rolling_whole_number_cycle:
    mov edx, num_of_bytes
    shr byte ptr [ebx+edx-1], 1              ; так как остатка нет, первый байт мы можем просто сместить вне цикла
    mov edx, 0                               ; устанавливаем регистр-остаток в 0
    adc edx, 0                               ; переносим бит-остаток в edx
    push edx                                 ; сохраняем edx в стек, чтобы другие операции не перезаписали его
    
    mov ecx, num_of_bytes                    ; первая итерация была на индексе 0, поэтому цикл начинается с 1
    dec ecx

    division_cycle:               
    shr byte ptr [ebx+ecx-1], 1              ; смещаем очередной байт
    mov esi, 0                               ; обнуляем ESI
    adc esi, 0                               ; сохранаяем CF в ESI
    pop edx                                  ; получаем остаток из регистра edx
    shl edx, 7                               ; смещаем его, так как остаток от прошлого байта стал старшим битом
    or byte ptr [ebx+ecx-1], dl              ; устанавливаем старший бит в байте на значение из остатка
    push esi                                 ; снова сохраняем esi в стек

    dec ecx                                  ; увеличиваем индекс итерации
    mov edx, 0                               ; кладём в edx количество итераций
    cmp ecx, edx                             ; сравниваем ecx с edx
    jge division_cycle                       ; если ecx > 0, то перейти к division_cycle
        
    pop edx                                  ; удаляем edx из стека после последней итерации
    
    mov ecx, [esp+8]                         ; перемещаем количество смещений из стека в ecx
    dec ecx                                  ; уменьшаем количество смещений
    mov [esp+8], ecx                         ; записываем значение обратно в переменную power
    mov edx, 0                               ; edx = 0                
    cmp ecx, edx                             ; сраниваем power с 0
    jg rolling_whole_number_cycle            ; если power > 0, то начать новую итерацию смещения

    ret 12                                   ; очищаем стек от аргументов
divide endp

print_bytes proc
    mov eax, [esp+4]                         ; помещаем указатель на массив в eax  
    mov ecx, num_of_bytes                    ; ecx = 0
   
    print_cycle:
    push ecx                                 ; сохраняем ecx
    push eax                                 ; сохраняем eax
    
    mov edx, 0                               ; обнуляем edx
    mov dl, [eax+ecx-1]                      ; dl = result[ecx]
    push edx                                 ; кладём значение байта в стек
    push offset fmt_string                   ; кладём указатель на форматирующую строку в стек
    call crt_printf                          ; выводим байт
    add esp, 8                               ; очищаем стек
    
    pop eax                                  ; восстанавливаем eax 
    pop ecx                                  ; восстанавливаем ecx

    dec ecx                                  ; уменьшаем индекс итерации
    mov edx, 0                               ; кладём в edx ограничение на количество итераций
    cmp ecx, edx                             ; сравниваем ecx с edx
    jg print_cycle                           ; если ecx < num_of_bytes, то перейти к print_cycle
    
    push offset new_line_string              ; кладём указатель на форматирующую строку в стек
    call crt_printf                          ; выводим байт
    add esp, 4                               ; очищаем стек
    
    ret 4
print_bytes endp

start:
    push offset result                       ; помещаем указатель на res в стек
    push power                               ; помещаем n в стек
    push offset source                       ; помещаем указатель на a в стек
    call divide                              ; вызываем функцию деления

    push offset source                       ; вывод source
    call print_bytes

    push offset result                       ; вывод result
    call print_bytes

    push 0
    call ExitProcess
end start