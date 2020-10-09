add_executable(
    main
    main.c
)

target_compile_options(
    main
    PRIVATE
    -Wall -Wextra -Wpedantic
)

target_link_libraries(
    main
    asound
)
