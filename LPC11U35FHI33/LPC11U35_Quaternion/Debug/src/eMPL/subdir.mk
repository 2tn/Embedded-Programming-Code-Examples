################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/eMPL/inv_mpu.c \
../src/eMPL/inv_mpu_dmp_motion_driver.c 

OBJS += \
./src/eMPL/inv_mpu.o \
./src/eMPL/inv_mpu_dmp_motion_driver.o 

C_DEPS += \
./src/eMPL/inv_mpu.d \
./src/eMPL/inv_mpu_dmp_motion_driver.d 


# Each subdirectory must supply rules for building sources it contributes
src/eMPL/%.o: ../src/eMPL/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DMPU9250 -D__CODE_RED -DCORE_M0 -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\LPC11U35_Quaternion\src\peripheral" -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\LPC11U35_Quaternion\inc" -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\CMSIS_CORE_LPC11Uxx\inc" -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\LPC11U35_Quaternion\src\eMPL" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


