

add_library (sqlLib "SqlConn.h" "SqlConn.cpp")

target_link_libraries(sqlLib mysqlcppconn fmt  )
