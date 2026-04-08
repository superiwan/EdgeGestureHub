cmake_minimum_required(VERSION 3.22)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake")
endif()

project(EdgeGestureHub C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(APP_ROOT ${PROJECT_ROOT}/cubemx/EdgeGestureHub)
set(NEAI_ROOT ${PROJECT_ROOT}/libneai_project-2026-03-15-16-00_2)
set(LINKER_SCRIPT ${PROJECT_ROOT}/cmake/STM32F407VETX_FLASH.ld)
set(STARTUP_FILE ${PROJECT_ROOT}/cmake/startup_stm32f407xx.s)

add_executable(${PROJECT_NAME}
    ${STARTUP_FILE}
    ${PROJECT_ROOT}/cmake/neai_compat.c
    ${APP_ROOT}/Core/Src/main.c
    ${APP_ROOT}/Core/Src/gpio.c
    ${APP_ROOT}/Core/Src/i2c.c
    ${APP_ROOT}/Core/Src/usart.c
    ${APP_ROOT}/Core/Src/stm32f4xx_it.c
    ${APP_ROOT}/Core/Src/stm32f4xx_hal_msp.c
    ${APP_ROOT}/Core/Src/system_stm32f4xx.c
    ${APP_ROOT}/Core/Src/app_task.c
    ${APP_ROOT}/Core/Src/control_logic.c
    ${APP_ROOT}/Core/Src/firewater.c
    ${APP_ROOT}/Core/Src/gesture_engine.c
    ${APP_ROOT}/Core/Src/imu_service.c
    ${APP_ROOT}/Core/Src/oled_service.c
    ${APP_ROOT}/Drivers/BSP/Src/mpu6050.c
    ${APP_ROOT}/Drivers/BSP/Src/ssd1306.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${APP_ROOT}/Core/Inc
    ${APP_ROOT}/Drivers/BSP/Inc
    ${APP_ROOT}/Drivers/CMSIS/Include
    ${APP_ROOT}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${APP_ROOT}/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${NEAI_ROOT}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    USE_HAL_DRIVER
    STM32F407xx
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -fno-short-enums
    -ffunction-sections
    -fdata-sections
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<CONFIG:Debug>:-Og -g3>
    $<$<CONFIG:Release>:-O2>
)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${LINKER_SCRIPT}
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -specs=nano.specs
    -specs=nosys.specs
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${NEAI_ROOT}/libneai.a
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)
