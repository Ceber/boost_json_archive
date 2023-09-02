# additional target to perform clang-format run, requires clang-format

# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h*)

set(FORMAT_STYLE '{BasedOnStyle: llvm, ColumnLimit: 130, UseTab: Never, IndentWidth: 2, TabWidth: 4}')

add_custom_target(
        format
        COMMAND /usr/bin/clang-format
        -style=${FORMAT_STYLE}
        -i
        ${ALL_SOURCE_FILES}
)

add_custom_target(
        check-format
        COMMAND /usr/bin/clang-format
        -style=${FORMAT_STYLE}
        --dry-run -Werror
        ${ALL_SOURCE_FILES}
)
