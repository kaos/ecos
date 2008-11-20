define bdm_preload
  set $mbar  = 0x10000001
  set $SCR   = $mbar - 1 + 0x0004
  set $SPR   = $mbar - 1 + 0x0006
  set $CSBR0 = $mbar - 1 + 0x0040
  set $CSOR0 = $mbar - 1 + 0x0044
  set $CSBR1 = $mbar - 1 + 0x0048
  set $CSOR1 = $mbar - 1 + 0x004C
  set $CSBR2 = $mbar - 1 + 0x0050
  set $CSOR2 = $mbar - 1 + 0x0054
  set $CSBR3 = $mbar - 1 + 0x0058
  set $CSOR3 = $mbar - 1 + 0x005C
  set $CSBR4 = $mbar - 1 + 0x0060
  set $CSOR4 = $mbar - 1 + 0x0064
  set $CSBR5 = $mbar - 1 + 0x0068
  set $CSOR5 = $mbar - 1 + 0x006C
  set $CSBR6 = $mbar - 1 + 0x0070
  set $CSOR6 = $mbar - 1 + 0x0074
  set $CSBR7 = $mbar - 1 + 0x0078
  set $CSOR7 = $mbar - 1 + 0x007C
  set $SDCR  = $mbar - 1 + 0x0180
  set $SDTR  = $mbar - 1 + 0x0184
  set *((short*) $SCR)   = 0x0083
  set *((short*) $SPR)   = 0xFFFF
  set *((int*)   $CSBR0) = 0xFFE00201
  set *((int*)   $CSOR0) = 0xFFE00014
  set *((int*)   $CSBR1) = 0
  set *((int*)   $CSOR1) = 0
  set *((int*)   $CSBR2) = 0
  set *((int*)   $CSOR2) = 0
  set *((int*)   $CSBR3) = 0
  set *((int*)   $CSOR3) = 0
  set *((int*)   $CSBR4) = 0
  set *((int*)   $CSOR4) = 0
  set *((int*)   $CSBR5) = 0
  set *((int*)   $CSOR5) = 0
  set *((int*)   $CSBR6) = 0
  set *((int*)   $CSOR6) = 0
  set *((int*)   $CSBR7) = 0x00000701
  set *((int*)   $CSOR7) = 0xFFC0007C
  set *((int*)   $SDTR)  = 0x0000F539
  set *((int*)   $SDCR)  = 0x00004211
  set *((int*)0)         = 0
end

define bdm_postload
  set $pc=0x20000
end
