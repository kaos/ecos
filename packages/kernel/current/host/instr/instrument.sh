#!/bin/sh
echo "/* This is an automatically generated file. Do not edit.               */"
echo "/* Repository name: PACKAGES/kernel/VERSION/include/instrument_desc.h  */"
echo "/* Install tree   : INSTALL/include/cyg/kernel/instrument_desc.h       */"
echo
echo "struct instrument_desc_s {                                "
echo "    char *   msg;                                         "
echo "    CYG_WORD num;                                         "
echo "};                                                        "
echo
echo struct instrument_desc_s instrument_desc[] = {

grep -e \#define.CYG_INSTRUMENT_EVENT -e \#define.CYG_INSTRUMENT_CLASS $1 | grep -v MAX | cut -d " " -f 2- | cut -d "_" -f 4- | sort | awk '{ print("{\""$1"\",   " $2 "},") }'

echo }\;
echo
echo "/* EOF instrument_desc.h */"

