add_executable(${APP_NAME} Main.cc)

target_include_directories(${APP_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${APP_NAME} PRIVATE
    ${MODULES}
    magic_enum::magic_enum
)