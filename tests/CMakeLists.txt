cmake_minimum_required(VERSION 3.16)

include_directories(${CMAKE_SOURCE_DIR}/src)

macro(add_vpms_test test_name test_source)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE
        vpms_lib
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Test
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro()

add_vpms_test(FeeServiceTest tst_feeservice.cpp)
add_vpms_test(AuthServiceTest tst_authservice.cpp)
add_vpms_test(PathPlannerTest tst_pathplanner.cpp)
add_vpms_test(DatabaseManagerTest tst_databasemanager.cpp)
add_vpms_test(PasswordHasherTest tst_passwordhasher.cpp)
