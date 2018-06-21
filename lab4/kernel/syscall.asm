;##############################################################
;
;@change		给新增系统调用编号
;				新增系统调用的参数传递
;
;@intent		系统调用
;
;################################################################*/

%include "sconst.inc"

INT_VECTOR_SYS_CALL equ 0x90  ;系統調用中斷號

_NR_printx	    equ 0
_NR_sendrec	    equ 1

;增加了以下内容 這些NR都是和global.c中的sys_call_table相對應的！
_NR_process_sleep	equ 2
_NR_new_disp_str	equ 3
_NR_sem_p       	equ 4
_NR_sem_v			equ 5
_NR_get_ticks		equ 6

; 导出符号
global	printx
global	sendrec

;增加了以下内容
global  process_sleep
global	new_disp_str
global  sem_p
global  sem_v
global 	get_ticks

bits 32
[section .text]

; ====================================================================================
;                          void printx(char* s);
; ====================================================================================
printx:
	mov	eax, _NR_printx
	mov	edx, [esp + 4]  ;edx 里保存了系统调用的参数
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================================
;                  sendrec(int function, int src_dest, MESSAGE* msg);
; ====================================================================================
; Never call sendrec() directly, call send_recv() instead.
sendrec:
	mov	eax, _NR_sendrec
	mov	ebx, [esp + 4]	; function
	mov	ecx, [esp + 8]	; src_dest
	mov	edx, [esp + 12]	; p_msg
	int	INT_VECTOR_SYS_CALL
	ret



;增加了以下内容
process_sleep:
	mov     eax,_NR_process_sleep
	mov		edx,[esp + 4]
	int		INT_VECTOR_SYS_CALL
	ret

new_disp_str:
	mov     eax,_NR_new_disp_str
	mov     edx,[esp+4]
	int 	INT_VECTOR_SYS_CALL
	ret

sem_p:  
    mov     eax,_NR_sem_p
	mov     edx,[esp+4]
	int 	INT_VECTOR_SYS_CALL
	ret
	
sem_v:      
	mov     eax,_NR_sem_v
	mov     edx,[esp+4]
	int 	INT_VECTOR_SYS_CALL
	ret
	
get_ticks:
	mov		eax, _NR_get_ticks
	int		INT_VECTOR_SYS_CALL
	ret
