# src目录的CMake配置

# 主程序
add_executable(class_management_system
    main.cpp
    core/user/user_manager.cpp
    core/notification/notification_manager.cpp
    core/class/class_manager.cpp
    core/study/study_manager.cpp
    core/finance/finance_manager.cpp
    core/discipline/discipline_manager.cpp
    core/activity/activity_manager.cpp
    database/database_connector.cpp
    database/query_builder.cpp
    utils/logger.cpp
    utils/config_loader.cpp
    utils/validator.cpp
    web/server.cpp
    web/routes.cpp
)

# 包含目录
target_include_directories(class_management_system PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${MYSQL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(class_management_system
    ${MYSQL_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    pthread
)

# 编译定义
target_compile_definitions(class_management_system PRIVATE
    ${MYSQL_DEFINITIONS}
)