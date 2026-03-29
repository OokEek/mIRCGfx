; v1.0 By Ook
;
; mIRCGfx.dll needed
; Uses mIRC v7.83+ /drawdll command
; Test Using: /testmircgfx
; You will need to select two image files in a format supported by mirc to test.
; once @pic window is open try clicking & dragging the mouse.
;
; alias to return dll path, NB: this is used by the dll too
alias mIRCGfxDll returnex mIRCGfx\mIRCGfx.dll

alias mIRCGfx returnex $dll($mIRCGfxDll,$$1,$2)

;copy <dst> <x> <y> <src> <x> <y> <w> <h> <alpha>
;rect <dst> <x> <y> <w> <h> <alpha> <colour>
;line <dst> <x> <y> <x2> <y2> <alpha> <colour>
;dot <dst> <x> <y> <alpha> <colour>
;gradient <dst> <alpha> <mode> <args>
;gradient <dst> <alpha> rect_h <x> <y> <w> <h> <colour1> <colour2>
;gradient <dst> <alpha> rect_v <x> <y> <w> <h> <colour1> <colour2>
;gradient <dst> <alpha> triangle <x> <y> <w> <h> <colour1> <colour2>

; $1 = cmd, $2 = dst picwin name, $3- = args
alias mIRCGfx_Draw drawdll -n $$2 $mIRCGfxDll DllDraw $$1-

alias -l mircgfx_drawbuffer {
  if (!$window($1)) return
  var %out = @outbuffer $+ $1
  if (!$window(%out)) window -hpfd %out -1 -1 $window($1).bw $window($1).bh

  if (!$window(@srcbuffer_one)) {
    var %f = %mircgfx_pic1
    if ($isfile(%f)) {
      window -hpfd @srcbuffer_one -1 -1 $window(%out).bw $window(%out).bh
      drawpic -sm @srcbuffer_one 0 0 $window(@srcbuffer_one).bw $window(@srcbuffer_one).bh %f
    }
  }
  if (!$window(@srcbuffer_two)) {
    var %f = %mircgfx_pic2
    if ($isfile(%f)) {
      window -hpfd @srcbuffer_two -1 -1 $window(%out).bw $window(%out).bh
      drawpic -sm @srcbuffer_two 0 0 $window(@srcbuffer_two).bw $window(@srcbuffer_two).bh %f
    }
  }
  if ($window(@srcbuffer_one)) drawcopy -n @srcbuffer_one 0 0 $window(@srcbuffer_one).bw $window(@srcbuffer_one).bh %out 0 0
  if ($window(@srcbuffer_two)) mIRCGfx_Draw copy %out 0 0 @srcbuffer_two 0 0 $window(@srcbuffer_two).bw $window(@srcbuffer_two).bh 128

  if ($2) set %mircgfx_mousex $calc($2 - 250)
  if (%mircgfx_mousex !isnum) set %mircgfx_mousex 0
  if ($3) set %mircgfx_mousey $calc($3 - 250)
  if (%mircgfx_mousey !isnum) set %mircgfx_mousey 0

  var %x = %mircgfx_mousex + 80, %y = %mircgfx_mousey + 80
  mIRCGfx_Draw rect %out %x %y 100 100 100 $rgb(255,0,0) 1

  var %x = %mircgfx_mousex + 130, %y = %mircgfx_mousey + 130
  mIRCGfx_Draw ellipse %out %x %y 100 100 100 $rgb(0,255,0)

  var %x1 = %mircgfx_mousex + 0, %y1 = %mircgfx_mousey + 0
  var %x2 = %mircgfx_mousex + 206, %y2 = %mircgfx_mousey + 124
  var %x3 = %mircgfx_mousex + 182, %y3 = %mircgfx_mousey + 95
  var %x4 = %mircgfx_mousex + 85, %y4 = %mircgfx_mousey + 12
  mIRCGfx_Draw chord %out %x1 %y1 %x2 %y2 %x3 %y3 %x4 %y4 100 $rgb(0,0,255)

  var %x = %mircgfx_mousex + 20, %y = %mircgfx_mousey + 20
  var %xr1 = %mircgfx_mousex + 135, %yr1 = %mircgfx_mousey + 12
  var %xr2 = %mircgfx_mousex + 182, %yr2 = %mircgfx_mousey + 95
  mIRCGfx_Draw pie %out %x %y 226 144 %xr1 %yr1 %xr2 %yr2 100 $rgb(0,255,0)

  var %x = %mircgfx_mousex + 180, %y = %mircgfx_mousey + 180
  mIRCGfx_Draw gradient %out 100 rect_h %x %y 200 200 $rgb(0,255,0) $rgb(255,0,0)

  var %x = %mircgfx_mousex + 280, %y = %mircgfx_mousey + 280
  mIRCGfx_Draw gradient %out 100 rect_v %x %y 200 200 $rgb(0,255,0) $rgb(255,0,0)

  var %x = %mircgfx_mousex + 380, %y = %mircgfx_mousey + 380
  mIRCGfx_Draw gradient %out 100 triangle %x %y 200 200 $rgb(0,255,0) $rgb(255,0,0)

  var %x1 = %mircgfx_mousex + 80, %y1 = %mircgfx_mousey + 80
  var %x2 = %mircgfx_mousex + 180, %y2 = %mircgfx_mousey + 180
  mIRCGfx_Draw line %out %x1 %y1 %x2 %y2 100 $rgb(0,0,255)

  var %x = %mircgfx_mousex + 90, %y = %mircgfx_mousey + 90
  mIRCGfx_Draw dot %out %x %y $rgb(0,255,0)

  drawdot %out
}
; $1 = @finalwindow
alias -l mircgfx_drawfinal {
  if (!$window($1)) return
  var %out = @outbuffer $+ $1
  if (!$window(%out)) return
  drawcopy -n %out 0 0 $window(%out).bw $window(%out).bh $1 0 0
  drawdot $1
}
alias -l mircgfx_drawframe {
  mircgfx_drawbuffer $1-
  mircgfx_drawfinal $1
}
alias -l mircgfx_cleanup {
  if ($window(@outbuffer@pic)) window -c $v1
  if ($window(@srcbuffer_one)) window -c $v1
  if ($window(@srcbuffer_two)) window -c $v1
  unset %mircgfx_mousex %mircgfx_mousey %mircgfx_drag
}
menu @pic {
  sclick: mircgfx_drawframe $menu $mouse.x $mouse.y
  mouse: {
    if ($mouse.key & 1) mircgfx_drawframe $menu $mouse.x $mouse.y
  }
  close: mircgfx_cleanup
}
alias testmircgfx {
  echo Version: $mIRCGfx(DllInfo)

  mircgfx_cleanup

  if (!%mircgfx_pic1) set %mircgfx_pic1 $qt($sfile($mircdir*.jpg,Select Picture One,Select))
  if (!%mircgfx_pic2) set %mircgfx_pic2 $qt($sfile($mircdir*.jpg,Select Picture Two,Select))

  if ($window(@pic)) window -c $v1
  window -ap @pic

  mircgfx_drawframe @pic
}

;
;
;
