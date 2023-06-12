################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/driver/lcdIli9341/ILI9341_GFX.c \
../Core/Src/driver/lcdIli9341/ILI9341_STM32_Driver.c 

OBJS += \
./Core/Src/driver/lcdIli9341/ILI9341_GFX.o \
./Core/Src/driver/lcdIli9341/ILI9341_STM32_Driver.o 

C_DEPS += \
./Core/Src/driver/lcdIli9341/ILI9341_GFX.d \
./Core/Src/driver/lcdIli9341/ILI9341_STM32_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/driver/lcdIli9341/%.o Core/Src/driver/lcdIli9341/%.su Core/Src/driver/lcdIli9341/%.cyclo: ../Core/Src/driver/lcdIli9341/%.c Core/Src/driver/lcdIli9341/subdir.mk
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-driver-2f-lcdIli9341

clean-Core-2f-Src-2f-driver-2f-lcdIli9341:
	-$(RM) ./Core/Src/driver/lcdIli9341/ILI9341_GFX.cyclo ./Core/Src/driver/lcdIli9341/ILI9341_GFX.d ./Core/Src/driver/lcdIli9341/ILI9341_GFX.o ./Core/Src/driver/lcdIli9341/ILI9341_GFX.su ./Core/Src/driver/lcdIli9341/ILI9341_STM32_Driver.cyclo ./Core/Src/driver/lcdIli9341/ILI9341_STM32_Driver.d ./Core/Src/driver/lcdIli9341/ILI9341_STM32_Driver.o ./Core/Src/driver/lcdIli9341/ILI9341_STM32_Driver.su

.PHONY: clean-Core-2f-Src-2f-driver-2f-lcdIli9341

