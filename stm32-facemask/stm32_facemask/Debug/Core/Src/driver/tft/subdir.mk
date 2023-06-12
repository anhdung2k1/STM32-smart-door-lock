################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/driver/tft/tft_spi.c 

OBJS += \
./Core/Src/driver/tft/tft_spi.o 

C_DEPS += \
./Core/Src/driver/tft/tft_spi.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/driver/tft/%.o Core/Src/driver/tft/%.su Core/Src/driver/tft/%.cyclo: ../Core/Src/driver/tft/%.c Core/Src/driver/tft/subdir.mk
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-driver-2f-tft

clean-Core-2f-Src-2f-driver-2f-tft:
	-$(RM) ./Core/Src/driver/tft/tft_spi.cyclo ./Core/Src/driver/tft/tft_spi.d ./Core/Src/driver/tft/tft_spi.o ./Core/Src/driver/tft/tft_spi.su

.PHONY: clean-Core-2f-Src-2f-driver-2f-tft

