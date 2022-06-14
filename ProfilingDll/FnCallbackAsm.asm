_DATA SEGMENT
	msg db "call entered",0
_DATA ENDS


extern FnEnter:PROC
extern FnLeave:PROC
extern FnTail:PROC


_TEXT	SEGMENT

PUBLIC InitEnterLeaveCallbacks

InitEnterLeaveCallbacks PROC
  ;mov pActivateEnterLeaveCallback, RCX
  ;mov pHashMap, RDX
  ;mov mapSize, R8D
  ret
InitEnterLeaveCallbacks ENDP


PUBLIC FnEnterCallback

FnEnterCallback PROC
  SUB RSP, 24
  MOV RDX, R9
  call FnEnter
  ADD RSP, 24
  ret
FnEnterCallback ENDP


PUBLIC FnLeaveCallback

FnLeaveCallback PROC
  SUB RSP, 24
  MOV RDX, R9
  call FnLeave
  ADD RSP, 24
  ret
FnLeaveCallback ENDP


PUBLIC FnTailCallback

FnTailCallback PROC
  SUB RSP, 24
  call FnTail
  ADD RSP, 24
  ret
FnTailCallback ENDP

_TEXT	ENDS

END