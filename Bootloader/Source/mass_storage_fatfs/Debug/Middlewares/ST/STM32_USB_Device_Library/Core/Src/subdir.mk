################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
../Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c 

OBJS += \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.o \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.o \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.o 

C_DEPS += \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.d \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.d \
./Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/STM32_USB_Device_Library/Core/Src/%.o: ../Middlewares/ST/STM32_USB_Device_Library/Core/Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -D__weak="__attribute__((weak))" -D__packed="__attribute__((__packed__))" -DUSE_HAL_DRIVER -DSTM32F103xE -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Inc" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Drivers/STM32F1xx_HAL_Driver/Inc" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Middlewares/Third_Party/FatFs/src/drivers" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Middlewares/ST/STM32_USB_Device_Library/Core/Inc" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Middlewares/Third_Party/FatFs/src" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Drivers/CMSIS/Include" -I"E:/Repositories/ARMaDa/FirmWare/trunk/Projects/BootloaderForARmadaV5/mass_storage_fatfs/Drivers/CMSIS/Device/ST/STM32F1xx/Include"  -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


