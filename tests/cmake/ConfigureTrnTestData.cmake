function(_mkdir dir)
    file(MAKE_DIRECTORY "${dir}")
endfunction()

function(configure_trn_test_data)
    # Directory and environment related content.
    set(TRN_TEST_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/test_data")
    set(TRN_TEST_HOME_DIR "${TRN_TEST_DATA_DIR}/home")
    set(TRN_TEST_HOME_DIR_CAPITALIZED "${TRN_TEST_DATA_DIR}/Home")
    _mkdir("${TRN_TEST_HOME_DIR}")
    _mkdir("${TRN_TEST_HOME_DIR}/News")
    set(TRN_TEST_TMP_DIR  "${TRN_TEST_DATA_DIR}/tmp")
    _mkdir("${TRN_TEST_TMP_DIR}")
    set(TRN_TEST_LOGIN_NAME "bozo")
    set(TRN_TEST_REAL_NAME "Bozo the Clown")
    set(TRN_TEST_DOT_DIR  "${TRN_TEST_DATA_DIR}/dot")
    _mkdir("${TRN_TEST_DOT_DIR}")
    set(TRN_TEST_TRN_DIR  "${TRN_TEST_DATA_DIR}/trn")
    _mkdir("${TRN_TEST_TRN_DIR}")
    set(TRN_TEST_LIB_DIR  "${TRN_TEST_DATA_DIR}/lib")
    _mkdir("${TRN_TEST_LIB_DIR}")
    set(TRN_TEST_RN_LIB_DIR "${TRN_TEST_DATA_DIR}/rn_lib")
    _mkdir("${TRN_TEST_RN_LIB_DIR}")
    set(TRN_TEST_THREAD_DIR "${TRN_TEST_RN_LIB_DIR}/threads")
    _mkdir("${TRN_TEST_THREAD_DIR}")
    set(TRN_TEST_LOCAL_HOST "huey.example.org")
    set(TRN_TEST_P_HOST_NAME "duey.example.org")
    set(TRN_TEST_NNTPSERVER "news.example.org")
    set(TRN_TEST_ORGANIZATION "Multi-cellular, biological")
    set(TRN_TEST_ORGFILE "${TRN_TEST_DATA_DIR}/organization")
    set(TRN_TEST_LOCAL_SPOOL_DIR "${TRN_TEST_DATA_DIR}/spool")
    _mkdir("${TRN_TEST_LOCAL_SPOOL_DIR}")

    # Newsgroup related content.
    set(TRN_TEST_SPOOL_DIR "${TRN_TEST_DATA_DIR}/spool")
    set(TRN_TEST_NEWSGROUP "alt.binaries.fractals")
    string(REPLACE "." "/" TRN_TEST_NEWSGROUP_SUBDIR "${TRN_TEST_NEWSGROUP}")
    set(TRN_TEST_NEWSGROUP_DIR "${TRN_TEST_SPOOL_DIR}/${TRN_TEST_NEWSGROUP_SUBDIR}")
    _mkdir("${TRN_TEST_NEWSGROUP_DIR}")

    # Article contents.
    set(TRN_TEST_ARTICLE_NUM 623)
    set(TRN_TEST_ARTICLE_FILE "${TRN_TEST_NEWSGROUP_DIR}/${TRN_TEST_ARTICLE_NUM}")
    set(TRN_TEST_HEADER_FROM_ADDRESS "bozo@clown-world.org")
    set(TRN_TEST_HEADER_FROM_NAME "Bozo the Clown")
    set(TRN_TEST_HEADER_FROM "${TRN_TEST_HEADER_FROM_NAME} <${TRN_TEST_HEADER_FROM_ADDRESS}>")
    set(TRN_TEST_HEADER_NEWSGROUPS "${TRN_TEST_NEWSGROUP},alt.flame")
    set(TRN_TEST_HEADER_FOLLOWUP_TO "alt.flame")
    set(TRN_TEST_HEADER_SUBJECT "Re: the best red nose")
    set(TRN_TEST_HEADER_REPLY_TO "Cyrus Longworth <clongworth@dark-shadows.example.org>")
    set(TRN_TEST_HEADER_DATE "Tue, 21 Jan 2003 01:21:09 +0000 (UTC)")
    set(TRN_TEST_HEADER_MESSAGE_ID "<b0i7a5$aoq$1@terabinaries.xmission.com>")
    set(TRN_TEST_HEADER_REFERENCES "nnrp.xmission xmission.general:21646")
    set(TRN_TEST_HEADER_X_BOOGIE_NIGHTS "oh what a night")
    set(TRN_TEST_HEADER_BYTES "2132")
    set(TRN_TEST_HEADER_LINES "16")
    set(TRN_TEST_BODY "Pneumatic tubes are killing the Internet.")
    set(TRN_TEST_SIGNATURE "XMission Internet Access - http://www.xmission.com - Voice: 801 539 0852")

    # Active file contents.
    set(TRN_TEST_NEWSGROUP_HIGH "${TRN_TEST_ARTICLE_NUM}")
    set(TRN_TEST_NEWSGROUP_LOW "${TRN_TEST_ARTICLE_NUM}")
    set(TRN_TEST_NEWSGROUP_PERM "y")

    # Access file contents
    set(TRN_TEST_ACCESS_LOCAL_ACTIVE_FILE   "${TRN_TEST_LOCAL_SPOOL_DIR}/active")
    set(TRN_TEST_ACCESS_LOCAL_ADD_GROUPS    "yes")
    set(TRN_TEST_ACCESS_LOCAL_NEWSRC        "${TRN_TEST_DOT_DIR}/local-newsrc")
    set(TRN_TEST_ACCESS_LOCAL_SPOOL_DIR     "${TRN_TEST_LOCAL_SPOOL_DIR}")
    set(TRN_TEST_ACCESS_NNTP_ACTIVE_FILE    "${TRN_TEST_HOME_DIR}/nntp-active")
    set(TRN_TEST_ACCESS_NNTP_ADD_GROUPS     "yes")
    set(TRN_TEST_ACCESS_NNTP_NEWSRC         "${TRN_TEST_DOT_DIR}/nntp-newsrc")
    set(TRN_TEST_ACCESS_NNTP_SERVER         "nntp.example.org")

    # Local newsrc contents
    math(EXPR _prev_article                     "${TRN_TEST_ARTICLE_NUM}-1")
    set(TRN_TEST_LOCAL_NEWSGROUP                "${TRN_TEST_NEWSGROUP}")
    set(TRN_TEST_LOCAL_NEWSGROUP_READ_ARTICLES  "1-${_prev_article}")

    # NNTP newsrc contents
    set(TRN_TEST_NNTP_NEWSGROUP                 "${TRN_TEST_NEWSGROUP}")
    set(TRN_TEST_NNTP_NEWSGROUP_READ_ARTICLES   "")

    # Generate test data files.
    configure_file(cmake/test_access_file.in            "${TRN_TEST_TRN_DIR}/access")
    configure_file(cmake/test_access_file.in            "${TRN_TEST_RN_LIB_DIR}/access.def")
    configure_file(cmake/test_active_file.in            "${TRN_TEST_SPOOL_DIR}/active")
    configure_file(cmake/test_article.in                "${TRN_TEST_ARTICLE_FILE}")
    configure_file(cmake/test_local_newsrc.in           "${TRN_TEST_ACCESS_LOCAL_NEWSRC}")
    configure_file(cmake/test_nntp_auth_file.in         "${TRN_TEST_DOT_DIR}/.nntpauth")
    configure_file(cmake/test_nntp_newsrc.in            "${TRN_TEST_ACCESS_NNTP_NEWSRC}")
    configure_file(cmake/test_organization_file.in      "${TRN_TEST_ORGFILE}")

    # Generate test source files.
    # test_config.h needs the size of the generated article.
    file(SIZE "${TRN_TEST_ARTICLE_FILE}" TRN_TEST_ARTICLE_SIZE)
    configure_file(cmake/test_config.h.in               test_config.h)
    configure_file(cmake/test_data.cpp.in               test_data.cpp)
endfunction()
