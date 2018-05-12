################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/fatfs/option/cc932.c \
../src/fatfs/option/unicode.c 

OBJS += \
./src/fatfs/option/cc932.o \
./src/fatfs/option/unicode.o 

C_DEPS += \
./src/fatfs/option/cc932.d \
./src/fatfs/option/unicode.d 


# Each subdirectory must supply rules for building sources it contributes
src/fatfs/option/%.o: ../src/fatfs/option/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -I"C:\Users\Yusuke\Documents\LPCXpresso_7.7.2_379\workspace\CMSIS_CORE_LPC11Uxx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


