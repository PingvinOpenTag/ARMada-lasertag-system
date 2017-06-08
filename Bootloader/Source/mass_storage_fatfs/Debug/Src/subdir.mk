################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/bsp_driver_sd.c \
../Src/fatfs.c \
../Src/isr-vector-stub.c \
../Src/main.c \
../Src/stm32f1xx_hal_msp.c \
../Src/stm32f1xx_it.c \
../Src/usb_device.c \
../Src/usbd_conf.c \
../Src/usbd_desc.c \
../Src/usbd_storage_if.c 

OBJS += \
./Src/bsp_driver_sd.o \
./Src/fatfs.o \
./Src/isr-vector-stub.o \
./Src/main.o \
./Src/stm32f1xx_hal_msp.o \
./Src/stm32f1xx_it.o \
./Src/usb_device.o \
./Src/usbd_conf.o \
./Src/usbd_desc.o \
./Src/usbd_storage_if.o 

C_DEPS += \
./Src/bsp_driver_sd.d \
./Src/fatfs.d \
./Src/isr-vector-stub.d \
./Src/main.d \
./Src/stm32f1xx_hal_msp.d \
./Src/stm32f1xx_it.d \
./Src/usb_device.d \
./Src/usbd_conf.d \
./Src/usbd_desc.d \
./Src/usbd_storage_if.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))" -DUSE_HAL_DRIVER -DSTM32F103xE -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Inc" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Drivers/STM32F1xx_HAL_Driver/Inc" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Middlewares/Third_Party/FatFs/src/drivers" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Middlewares/Third_Party/FatFs/src" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Drivers/CMSIS/Include" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Drivers/CMSIS/Device/ST/STM32F1xx/Include"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


