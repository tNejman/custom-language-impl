add_executable(${APP_NAME} Main.cc)

target_link_libraries(${APP_NAME} PRIVATE
    ${MODULES}
    magic_enum::magic_enum
)