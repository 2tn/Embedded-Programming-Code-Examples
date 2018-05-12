################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/LPC11U35_Quaternion.c \
../src/MotionDriver.c \
../src/cr_startup_lpc11uxx.c \
../src/crp.c 

OBJS += \
./src/LPC11U35_Quaternion.o \
./src/MotionDriver.o \
./src/cr_startup_lpc11uxx.o \
./src/crp.o 

C_DEPS += \
./src/LPC11U35_Quaternion.d \
./src/MotionDriver.d \
./src/cr_startup_lpc11uxx.d \
./src/crp.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -DMPU9250 -D__CODE_RED -DCORE_M0 -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\LPC11U35_Quaternion\src\peripheral" -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\LPC11U35_Quaternion\inc" -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\CMSIS_CORE_LPC11Uxx\inc" -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\LPC11U35_Quaternion\src\eMPL" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


