!include "MUI.nsh"

Name "CeB Alpha"
OutFile "CeB_alpha.exe"

Var EXECCEB

InstallDir "$PROGRAMFILES\CeB_Alpha"

InstallDirRegKey HKCU "Software\CeB Alpha" ""

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "CeB\LICENCE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
Page custom ExecutePage
  
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

ReserveFile "execute.ini"
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; Installer Sections
Section "CeB Alpha" SecCeB

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  File "CeB\ceb.exe"
  File "CeB\mingwm10.dll"
  File "CeB\lua51.dll"
  File "CeB\QtCore4.dll"
  File "CeB\QtGui4.dll"
  File "CeB\QtNetwork4.dll"
  File "CeB\QtXml4.dll"
  File /nonfatal /r "CeB\logs"
  File /r "CeB\languages"
  File "CeB\whatsnew.html"
  File /r "CeB\scripts"
  File /r "CeB\modifiers"
  SetOverwrite off
  File /nonfatal /r "CeB\profiles"
  File "CeB\config.xml"
  File /r "CeB\resources"
  SetOverwrite on
  
  ;Store installation folder
  WriteRegStr HKCU "Software\CeB Alpha" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ;Delete old stuff
  delete "$INSTDIR\nexus.exe"
  delete "$INSTDIR\languages\nexus_fr.qm"

SectionEnd

Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\CeB Alpha"
  CreateShortCut "$SMPROGRAMS\CeB Alpha\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\CeB Alpha\CeB Alpha.lnk" "$INSTDIR\ceb.exe" "" "$INSTDIR\ceb.exe" 0
  
SectionEnd

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  ; Remove files and uninstaller
  Delete $INSTDIR\ceb.exe
  Delete $INSTDIR\mingwm10.dll
  Delete $INSTDIR\lua51.dll
  Delete $INSTDIR\QtCore4.dll
  Delete $INSTDIR\QtGui4.dll
  Delete $INSTDIR\QtNetwork4.dll
  Delete $INSTDIR\QtXml4.dll
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\scripts
  Delete $INSTDIR\modifiers
  Delete $INSTDIR\resources

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\CeB Alpha\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\CeB Alpha"

  RMDir "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\CeB Alpha"

SectionEnd

Function .onInit
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "execute.ini" 
FunctionEnd

LangString TEXT_IO_TITLE ${LANG_ENGLISH} "Options after install"
LangString TEXT_IO_SUBTITLE ${LANG_ENGLISH} "Choose what to do after install."

Function ExecutePage

  !insertmacro MUI_HEADER_TEXT "$(TEXT_IO_TITLE)" "$(TEXT_IO_SUBTITLE)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "execute.ini"
  !insertmacro MUI_INSTALLOPTIONS_READ $EXECCEB "execute.ini" "Field 1" "State"

  IntCmp $EXECCEB 0 End

  Exec '"$INSTDIR\ceb.exe"'

End:

FunctionEnd
