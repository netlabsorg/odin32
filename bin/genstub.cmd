/* rexx */

parse arg szFileDef

if szFileDef='' then
do
  say 'Usage: GENSTUB <.def>'
  exit
end


/* open .def file and parse for ";" */
ulLine=0

do while LINES(szFileDef)
  szLine=LINEIN(szFileDef)
  ulLine=ulLine+1
  
  /* parse */
  if WORD(szLine,1)=';' then
  do
    szFunction = WORD(szLine,2)
    stub_write(szFunction)
  end
end
exit



stub_write: procedure
  parse arg szName

  say'/*****************************************************************************'
  say' * Name      : '||szName
  say' * Purpose   : '
  say' * Parameters: '
  say' * Variables : '
  say' * Result    : '
  say' * Remark    : '
  say' * Status    : UNTESTED STUB'
  say' *'
  say' * Author    : Patrick Haller [Tue, 1998/06/16 23:00]'
  say' *****************************************************************************/'
  say''
  say'DWORD WIN32API OS2'||szName||'()'
  say'{'
  say'  dprintf(("ADVAPI32: '||szName||'() not implemented.\n",'
  say'           ));'
  say''
  say'  return (0);'
  say'}'
  say''
  say''
return''