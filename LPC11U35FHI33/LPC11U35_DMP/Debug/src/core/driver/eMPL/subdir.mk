################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/core/driver/eMPL/inv_mpu.c \
../src/core/driver/eMPL/inv_mpu_dmp_motion_driver.c 

OBJS += \
./src/core/driver/eMPL/inv_mpu.o \
./src/core/driver/eMPL/inv_mpu_dmp_motion_driver.o 

C_DEPS += \
./src/core/driver/eMPL/inv_mpu.d \
./src/core/driver/eMPL/inv_mpu_dmp_motion_driver.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/driver/eMPL/%.o: ../src/core/driver/eMPL/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -DMPL_LOG_NDEBUG=1 -DMPU9250 -DEMPL -DUSE_DMP -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\CMSIS_CORE_LPC11Uxx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


