cmake_minimum_required(VERSION 3.0.0)

set(PROJ "firmware")
set(MCU "STM32F103xB")
project (${PROJ} VERSION 0.1.0)

# https://developer.arm.com/open-source/gnu-toolchain/gnu-rm.
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")

# Library dirs 
set(Core_DIR ${CMAKE_SOURCE_DIR}/Core)
set(Drivers_DIR ${CMAKE_SOURCE_DIR}/Drivers)
set(CMSIS_DIR ${CMAKE_SOURCE_DIR}/Drivers/CMSIS)
set(HAL_DIR ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver)
set(FreeRTOS_DIR ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS)

# Includes 
include_directories(${Core_DIR}/Inc)
include_directories(${HAL_DIR}/Inc)
include_directories(${HAL_DIR}/Inc/Legacy)
include_directories(${FreeRTOS_DIR}/Source/include)
include_directories(${FreeRTOS_DIR}/Source/CMSIS_RTOS)
include_directories(${FreeRTOS_DIR}/Source/portable/GCC/ARM_CM3)
include_directories(${CMSIS_DIR}/Device/ST/STM32F1xx/Include)
include_directories(${CMSIS_DIR}/Include)
# USB
include_directories(USB_DEVICE/App)
include_directories(USB_DEVICE/Target)
include_directories(Middlewares/ST/STM32_USB_Device_Library/Core/Inc)
include_directories(Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc)

# Sources
file(GLOB Core_SRC
    "${Core_DIR}/Src/*.c"
)

file(GLOB CMSIS_SRC
    "${CMSIS_DIR}/**/*.c"
)

file(GLOB HAL_SRC
    ${HAL_DIR}/Src/stm32f1xx_hal_gpio_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_tim.c
    ${HAL_DIR}/Src/stm32f1xx_hal_tim_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal.c
    ${HAL_DIR}/Src/stm32f1xx_hal_rcc.c
    ${HAL_DIR}/Src/stm32f1xx_hal_rcc_ex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_gpio.c
    ${HAL_DIR}/Src/stm32f1xx_hal_dma.c
    ${HAL_DIR}/Src/stm32f1xx_hal_cortex.c
    ${HAL_DIR}/Src/stm32f1xx_hal_pwr.c
    ${HAL_DIR}/Src/stm32f1xx_hal_flash.c
    ${HAL_DIR}/Src/stm32f1xx_hal_flash_ex.c
#    ${HAL_DIR}/Src/stm32f1xx_hal_uart.c
    ${HAL_DIR}/Src/stm32f1xx_hal_exti.c

    # USB
    ${HAL_DIR}/Src/stm32f1xx_hal_pcd.c
    ${HAL_DIR}/Src/stm32f1xx_hal_pcd_ex.c
    ${HAL_DIR}/Src/stm32f1xx_ll_usb.c

    USB_DEVICE/App/usb_device.c
    USB_DEVICE/App/usbd_desc.c
    USB_DEVICE/App/usbd_cdc_if.c
    USB_DEVICE/Target/usbd_conf.c

    Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
    Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
    Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
    Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
)

file(GLOB FreeRTOS_SRC
    "${FreeRTOS_DIR}/Source/CMSIS_RTOS/cmsis_os.c"
    "${FreeRTOS_DIR}/Source/portable/GCC/ARM_CM3/port.c"
    "${FreeRTOS_DIR}/Source/*.c"
)

# FLAGS
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")

set(CMAKE_C_FLAGS "")

set(CMAKE_C_FLAGS "-D${MCU} -DUSE_HAL_DRIVER")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -o0 -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -T${CMAKE_SOURCE_DIR}/STM32F103C8Tx_FLASH.ld")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --specs=nosys.specs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --specs=nano.specs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--gc-sections -static")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb -Wl,--start-group -lc -lm -Wl,--end-group")

set_source_files_properties(startup_stm32f103xb.s PROPERTIES LANGUAGE C)
add_executable(${PROJ}.elf
    startup_stm32f103xb.s
    Firmware/blinking_task.c
    ${HAL_SRC}
    ${Core_SRC}
    ${FreeRTOS_SRC}
)

add_custom_command(
  TARGET ${PROJ}.elf
  COMMAND "arm-none-eabi-size"
  ARGS "${PROJ}.elf"
)

add_custom_command(
  TARGET ${PROJ}.elf
  COMMAND "arm-none-eabi-objdump"
  ARGS "-S" "${PROJ}.elf" ">>" "${PROJ}.lst"
)

# Flash firmware to target.
add_custom_target(flash
    openocd -f ${CMAKE_SOURCE_DIR}/openocd/blue_pill.cfg
    -c "flash write_image erase ${PROJ}.elf"
    -c "reset run"
    -c "shutdown"
    DEPENDS ${PROJ}.elf
    COMMENT "Flashing target hardware"
)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
