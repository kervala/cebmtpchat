FIND_PACKAGE(Lua51 REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${LUA_INCLUDE_DIR})

FILE(GLOB HEADER src/*.h)
FILE(GLOB SRC src/*.cpp *.rc)
FILE(GLOB UI ui/*.ui)
FILE(GLOB QRC *.qrc)
FILE(GLOB TS *.ts)

# Generate .cpp from .qrc
QT4_ADD_RESOURCES(QRC_SRC ${QRC})

# Generate .h from .ui
QT4_WRAP_UI(UI_HEADERS ${UI})

# Generate .cxx from .h
QT4_WRAP_CPP(MOC_SRC ${HEADER})

IF(WITH_UPDATE_TRANSLATIONS)
  # Create or update TS files and generate QM files
  SET(TRANS ${SRC} ${UI} ${HEADER})
  QT4_CREATE_TRANSLATION(QM ${TRANS} ${TS})
ELSE(WITH_UPDATE_TRANSLATIONS)
  # Only generate QM files
  QT4_ADD_TRANSLATION(QM ${TS})
ENDIF(WITH_UPDATE_TRANSLATIONS)

SOURCE_GROUP("generated" FILES ${UI_HEADERS} ${QRC_SRC} ${MOC_SRC})
SOURCE_GROUP("res" FILES ${QRC})
SOURCE_GROUP("include" FILES ${HEADER})
SOURCE_GROUP("src" FILES ${SRC})
SOURCE_GROUP("ui" FILES ${UI})
SOURCE_GROUP("ts" FILES ${TS} ${QM})

SET_TARGET_GUI_EXECUTABLE(${TARGET} ${SRC} ${QRC_SRC} ${MOC_SRC} ${UI_HEADERS} ${HEADER} ${TS})
TARGET_LINK_LIBRARIES(${TARGET} max ${QT_LIBRARIES} ${LUA_LIBRARIES})
SET_DEFAULT_PROPS(${TARGET} ${PRODUCT})

ADD_DEFINITIONS(${QT_DEFINITIONS})

IF(MINGW)
	# resource compilation for MinGW
	ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ceb_rc.o
		COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/ceb.rc 
		-o ${CMAKE_CURRENT_BINARY_DIR}/ceb_rc.o )
	SET(CEB_SRCS ${CEB_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/ceb_rc.o)
ENDIF(MINGW)

# translations
ADD_CUSTOM_TARGET("${TARGET}_translations" ALL DEPENDS ${QM})

IF(WITH_PCH)
  ADD_NATIVE_PRECOMPILED_HEADER(${TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/src/common.h ${CMAKE_CURRENT_SOURCE_DIR}/src/common.cpp)
ENDIF(WITH_PCH)

INSTALL(FILES ${QM} DESTINATION share/ceb/languages)
INSTALL(DIRECTORY share/ceb DESTINATION share PATTERN .svn EXCLUDE)

IF(UNIX AND NOT APPLE)
  INSTALL(FILES images/ceb.xpm DESTINATION share/pixmaps)
  INSTALL(FILES images/ceb_16x16.png DESTINATION share/icons/hicolor/16x16/apps RENAME ceb.png)
  INSTALL(FILES images/ceb_22x22.png DESTINATION share/icons/hicolor/22x22/apps RENAME ceb.png)
  INSTALL(FILES images/ceb_24x24.png DESTINATION share/icons/hicolor/24x24/apps RENAME ceb.png)
  INSTALL(FILES images/ceb_32x32.png DESTINATION share/icons/hicolor/32x32/apps RENAME ceb.png)
  INSTALL(FILES images/ceb_48x48.png DESTINATION share/icons/hicolor/48x48/apps RENAME ceb.png)
  INSTALL(FILES images/ceb_128x128.png DESTINATION share/icons/hicolor/128x128/apps RENAME ceb.png)
  INSTALL(FILES images/ceb.svg DESTINATION share/icons/hicolor/scalable/apps RENAME ceb.svg)
  INSTALL(FILES share/applications/ceb.desktop DESTINATION share/applications)
ENDIF(UNIX AND NOT APPLE)
