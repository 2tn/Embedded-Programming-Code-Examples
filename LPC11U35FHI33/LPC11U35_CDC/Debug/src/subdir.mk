################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/aeabi_romdiv_patch.s 

C_SRCS += \
../src/cdc_desc.c \
../src/cr_startup_lpc11uxx.c \
../src/crp.c \
../src/main.c \
../src/vcom.c 

OBJS += \
./src/aeabi_romdiv_patch.o \
./src/cdc_desc.o \
./src/cr_startup_lpc11uxx.o \
./src/crp.o \
./src/main.o \
./src/vcom.o 

C_DEPS += \
./src/cdc_desc.d \
./src/cr_startup_lpc11uxx.d \
./src/crp.d \
./src/main.d \
./src/vcom.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\CMSIS_CORE_LPC11Uxx\inc" -g3 -mcpu=cortex-m0 -mthumb -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\CMSIS_CORE_LPC11Uxx\inc" -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\LPC11U35_CDC\inc\cdc" -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\LPC11U35_CDC\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


