cmake_minimum_required(VERSION 3.10)

project(
    midi_split
    VERSION 0.1
    LANGUAGES C
)

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(app)
