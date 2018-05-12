################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: LPC11U35_NEO-7M_I2C.axf

# Tool invocations
LPC11U35_NEO-7M_I2C.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Yusuke\Documents\LPCXpresso_8.1.4_606\workspace\CMSIS_CORE_LPC11Uxx\Debug" -Xlinker -Map="LPC11U35_NEO-7M_I2C.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "LPC11U35_NEO-7M_I2C_Debug.ld" -o "LPC11U35_NEO-7M_I2C.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) LPC11U35_NEO-7M_I2C.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "LPC11U35_NEO-7M_I2C.axf"; # arm-none-eabi-objcopy -v -O binary "LPC11U35_NEO-7M_I2C.axf" "LPC11U35_NEO-7M_I2C.bin" ; # checksum -p LPC11U35_501 -d "LPC11U35_NEO-7M_I2C.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
