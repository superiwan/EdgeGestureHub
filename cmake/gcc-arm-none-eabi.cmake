set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(_toolchain_search_roots
    "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.2 rel1/bin"
    "$ENV{LOCALAPPDATA}/stm32cube/bundles/gnu-tools-for-stm32/14.3.1+st.2/bin"
)

find_program(CMAKE_C_COMPILER arm-none-eabi-gcc HINTS ${_toolchain_search_roots} REQUIRED)
find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc HINTS ${_toolchain_search_roots} REQUIRED)
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy HINTS ${_toolchain_search_roots} REQUIRED)
find_program(CMAKE_SIZE arm-none-eabi-size HINTS ${_toolchain_search_roots} REQUIRED)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

