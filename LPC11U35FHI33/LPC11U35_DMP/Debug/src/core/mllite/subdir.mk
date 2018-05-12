################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/core/mllite/data_builder.c \
../src/core/mllite/hal_outputs.c \
../src/core/mllite/message_layer.c \
../src/core/mllite/ml_math_func.c \
../src/core/mllite/mlmath.c \
../src/core/mllite/mpl.c \
../src/core/mllite/results_holder.c \
../src/core/mllite/start_manager.c \
../src/core/mllite/storage_manager.c 

OBJS += \
./src/core/mllite/data_builder.o \
./src/core/mllite/hal_outputs.o \
./src/core/mllite/message_layer.o \
./src/core/mllite/ml_math_func.o \
./src/core/mllite/mlmath.o \
./src/core/mllite/mpl.o \
./src/core/mllite/results_holder.o \
./src/core/mllite/start_manager.o \
./src/core/mllite/storage_manager.o 

C_DEPS += \
./src/core/mllite/data_builder.d \
./src/core/mllite/hal_outputs.d \
./src/core/mllite/message_layer.d \
./src/core/mllite/ml_math_func.d \
./src/core/mllite/mlmath.d \
./src/core/mllite/mpl.d \
./src/core/mllite/results_holder.d \
./src/core/mllite/start_manager.d \
./src/core/mllite/storage_manager.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/mllite/%.o: ../src/core/mllite/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M0 -D__USE_CMSIS=CMSIS_CORE_LPC11Uxx -D__LPC11UXX__ -D__REDLIB__ -DMPL_LOG_NDEBUG=1 -DMPU9250 -DEMPL -DUSE_DMP -I"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\CMSIS_CORE_LPC11Uxx\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


