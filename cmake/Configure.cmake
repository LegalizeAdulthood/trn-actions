include(CheckIncludeFile)
include(CheckSymbolExists)

#
# Perform inspections of the system and configure accordingly.
#
function(configure)
    set(DIRENTRYTYPE "struct dirent")
    set(EMULATE_NDIR ON)
    check_symbol_exists(ftime "sys/types.h;sys/timeb.h" HAS_FTIME)
    check_include_file(unistd.h I_UNISTD)
    if(I_UNISTD)
	check_symbol_exists(getcwd "unistd.h" HAS_GETCWD)
	check_symbol_exists(gethostname "unistd.h" HAS_GETHOSTNAME)
    endif()
    if(NOT HAS_GETCWD)
	check_symbol_exists(getcwd "direct.h" HAS_GETCWD)
    endif()
    if(NOT HAS_GETHOSTNAME)
	set(CMAKE_REQUIRED_LIBRARIES "ws2_32.lib")
	check_symbol_exists(gethostname "winsock2.h" HAS_GETHOSTNAME)
	unset(CMAKE_REQUIRED_LIBRARIES)
    endif()
#    check_include_file(sys/stat.h I_SYS_STAT)
#    if(I_SYS_STAT)
#	check_symbol_exists(mkdir "sys/stat.h" HAS_MKDIR)
#    endif()
#    if(NOT HAS_MKDIR)
#	check_symbol_exists(mkdir "direct.h" HAS_MKDIR)
#    endif()
    set(HAS_MKDIR ON)
    set(HAS_RENAME ON)
    set(HAS_TERMLIB ON)
    set(I_TIME ON)
    set(MBOX_CHAR "F")
    set(MSDOS ON)
    set(NEWS_ADMIN "root")
    set(NORMSIG ON)
    set(ROOT_ID "0")
    set(SCAN ON)
    set(SCORE ON)
    set(SERVER_NAME "news.gmane.io")
    set(SIGNAL_T "void")
    set(SUPPORT_NNTP ON)
    set(SUPPORT_XTHREAD ON)
    set(TRN_INIT OFF)
    set(TRN_SELECT ON)
    set(USE_GENAUTH ON)
    configure_file(config.h.in config.h)
endfunction()
