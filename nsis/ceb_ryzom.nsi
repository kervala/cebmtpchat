!include "MUI.nsh"

Name "CeB"
OutFile "ceb_ryzom_20080722.exe"

Var EXECCEB

InstallDir "$PROGRAMFILES\CeB"

InstallDirRegKey HKCU "Software\CeB" ""

; Uninstall regkey
!define uninstroot "Software\Microsoft\Windows\CurrentVersion\Uninstall"
!define uninstkey "${uninstroot}\CeB"

SetCompressor /SOLID LZMA

;--------------------------------
;Language Selection Dialog Settings

;Remember the installer language
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY "Software\CeB"
!define MUI_LANGDLL_REGISTRY_VALUENAME "Language"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\CeB\share\CeB\LICENCE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
Page custom ExecutePage
  
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"

;These files should be inserted before other files in the data block
;Keep these lines before any File command
;Only for solid compression (by default, solid compression is enabled for BZIP2 and LZMA)
  
!insertmacro MUI_RESERVEFILE_LANGDLL

ReserveFile "execute.ini"
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

;Language strings

LangString ProfileFile  ${LANG_GERMAN} "leanon_default.xml"
LangString ProfileFile  ${LANG_FRENCH}  "aniro_default.xml"
LangString ProfileFile  ${LANG_ENGLISH} "arispotle_default.xml"

; Installer Sections
Section "CeB" SecCeB

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  File "..\CeB\bin\ceb.exe"
  File "..\CeB\bin\lua51.dll"
  File "..\CeB\bin\Microsoft.VC90.CRT.manifest"
  File "..\CeB\bin\msvcp90.dll"
  File "..\CeB\bin\msvcr90.dll"
  File /r "..\CeB\bin\languages"
  File "..\CeB\bin\whatsnew.html"
  File "..\CeB\bin\aniro_default.xml"
  File "..\CeB\bin\arispotle_default.xml"
  File "..\CeB\bin\leanon_default.xml"
  File /r "..\CeB\bin\scripts"
  File /r "..\CeB\bin\modifiers"

  SetOverwrite off

  File /r "..\CeB\bin\resources"

  SetOverwrite on
  
  ;Store installation folder
  WriteRegStr HKCU "Software\CeB" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  CreateDirectory $INSTDIR\profiles

  Delete $INSTDIR\profiles\default.xml
  
  Rename $(ProfileFile) $INSTDIR\profiles\default.xml

  ;Ajoute une entrée dans Ajout/Supression de Programmes
  WriteRegStr HKLM "${uninstkey}" "DisplayIcon" "$INSTDIR\CeB.exe,0"
  WriteRegStr HKLM "${uninstkey}" "DisplayName" "CeB"
;  WriteRegStr HKLM "${uninstkey}" "DisplayVersion" "${version}"
  WriteRegStr HKLM "${uninstkey}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "${uninstkey}" "QuietUninstallString" '"$INSTDIR\Uninstall.exe" /S'
  WriteRegStr HKLM "${uninstkey}" "UninstallString" "$INSTDIR\Uninstall.exe"
;  WriteRegStr HKLM "${uninstkey}" "Publisher" "${companyreg}"
;  WriteRegStr HKLM "${uninstkey}" "HelpLink" "${website}"
;  WriteRegStr HKLM "${uninstkey}" "URLInfoAbout" "${website}"
SectionEnd

Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\CeB"
  CreateShortCut "$SMPROGRAMS\CeB\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\CeB\CeB.lnk" "$INSTDIR\ceb.exe" "" "$INSTDIR\ceb.exe" 0
  
SectionEnd

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  ; Remove files and uninstaller
  Delete $INSTDIR\ceb.exe
  Delete $INSTDIR\lua51.dll
  Delete $INSTDIR\Microsoft.VC90.CRT.manifest
  Delete $INSTDIR\msvcp90.dll
  Delete $INSTDIR\msvcr90.dll
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\scripts
  Delete $INSTDIR\modifiers
  Delete $INSTDIR\resources
  Delete $INSTDIR\profiles

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\CeB\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\CeB"

  RMDir /r "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\CeB"

  ;remove the section to Add/Remove Programs
  DeleteRegKey HKLM "${uninstkey}"

SectionEnd

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "execute.ini" 

FunctionEnd

LangString TEXT_IO_TITLE ${LANG_ENGLISH} "Options after install"
LangString TEXT_IO_TITLE ${LANG_FRENCH} "Options après l'installation"
LangString TEXT_IO_TITLE ${LANG_GERMAN} "Options after install"
LangString TEXT_IO_SUBTITLE ${LANG_ENGLISH} "Choose what to do after install."
LangString TEXT_IO_SUBTITLE ${LANG_FRENCH} "Choisir une option après l'installation."
LangString TEXT_IO_SUBTITLE ${LANG_GERMAN} "Choose what to do after install."

Function ExecutePage

  !insertmacro MUI_UNGETLANGUAGE
  !insertmacro MUI_HEADER_TEXT "$(TEXT_IO_TITLE)" "$(TEXT_IO_SUBTITLE)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "execute.ini"
  !insertmacro MUI_INSTALLOPTIONS_READ $EXECCEB "execute.ini" "Field 1" "State"

  IntCmp $EXECCEB 0 End

  Exec '"$INSTDIR\ceb.exe"'

End:

FunctionEnd
