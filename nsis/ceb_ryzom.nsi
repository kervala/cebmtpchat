!include "MUI.nsh"

Name "CeB"
OutFile "ceb_ryzom_20080727.exe"

RequestExecutionLevel admin

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

  ;dlls
  File "..\CeB\bin\lua51.dll"
  File "..\CeB\bin\Microsoft.VC90.CRT.manifest"
  File "..\CeB\bin\msvcp90.dll"
  File "..\CeB\bin\msvcr90.dll"

  ;docs
  File "..\ceb\share\ceb\whatsnew.html"

  ;default profiles
  File "..\ceb\share\ceb\aniro_default.xml"
  File "..\ceb\share\ceb\arispotle_default.xml"
  File "..\ceb\share\ceb\leanon_default.xml"
  
  ;languages
  SetOutPath "$INSTDIR\languages"
  
  File "..\ceb\share\ceb\languages\ceb_de.qm"
  File "..\ceb\share\ceb\languages\ceb_fr.qm"
;  File "..\ceb\share\ceb\languages\ceb_nl.qm"
  File "..\ceb\share\ceb\languages\ceb_us.qm"
;  File "..\ceb\share\ceb\languages\ceb_pt-br.qm"

  ;scripts
  SetOutPath "$INSTDIR\scripts"
  File "..\ceb\share\ceb\scripts\calc.lua"

  SetOutPath "$INSTDIR\modifiers"
  File "..\ceb\share\ceb\modifiers\event.lua"
  File "..\ceb\share\ceb\modifiers\manual.html"
  File "..\ceb\share\ceb\modifiers\mtpsays.lua"
  File "..\ceb\share\ceb\modifiers\someoneasksyou.lua"
  File "..\ceb\share\ceb\modifiers\someonereplies.lua"
  File "..\ceb\share\ceb\modifiers\someonesays.lua"
  File "..\ceb\share\ceb\modifiers\someonetellsyou.lua"
  File "..\ceb\share\ceb\modifiers\utils.lua"

  ;resources
  SetOutPath "$INSTDIR\resources"
  File "..\ceb\share\ceb\resources\notify.wav"

  Delete $INSTDIR\profiles\default.xml

  CreateDirectory $INSTDIR\profiles

  Rename "$INSTDIR\$(ProfileFile) "$INSTDIR\profiles\default.xml"

  ;Store installation folder
  WriteRegStr HKCU "Software\CeB" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ;Ajoute une entr�e dans Ajout/Supression de Programmes
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
LangString TEXT_IO_TITLE ${LANG_FRENCH} "Options apr�s l'installation"
LangString TEXT_IO_TITLE ${LANG_GERMAN} "Options after install"
LangString TEXT_IO_SUBTITLE ${LANG_ENGLISH} "Choose what to do after install."
LangString TEXT_IO_SUBTITLE ${LANG_FRENCH} "Choisir une option apr�s l'installation."
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
