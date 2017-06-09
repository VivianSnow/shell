cmake_minimum_required(VERSION 3.6)
project(shell)

set(CMAKE_C_STANDARD 99)
set(SOURCE_FILES main.c readline_and_history.c readline_and_history.h export.c export.h utils.c utils.h job.c job.h signal_handle.h signal_handle.c redirect.c redirect.h)
add_executable(shell ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(shell -lreadline)
