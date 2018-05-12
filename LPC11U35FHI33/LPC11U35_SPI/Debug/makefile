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
all: LPC11U35_SPI.axf

# Tool invocations
LPC11U35_SPI.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Yusuke\Documents\LPCXpresso_7.7.2_379\workspace\CMSIS_CORE_LPC11Uxx\Debug" -Xlinker -Map="LPC11U35_SPI.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "LPC11U35_SPI_Debug.ld" -o "LPC11U35_SPI.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) LPC11U35_SPI.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "LPC11U35_SPI.axf"; # arm-none-eabi-objcopy -v -O binary "LPC11U35_SPI.axf" "LPC11U35_SPI.bin" ; # checksum -p LPC11U35_501 -d "LPC11U35_SPI.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
