################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include newlib/subdir.mk
-include L5_Application/source/cmd_handlers/subdir.mk
-include L5_Application/source/subdir.mk
-include L5_Application/periodic_scheduler/subdir.mk
-include L5_Application/examples/subdir.mk
-include L5_Application/subdir.mk
-include L4_IO/wireless/src/subdir.mk
-include L4_IO/src/subdir.mk
-include L4_IO/fat/option/subdir.mk
-include L4_IO/fat/disk/subdir.mk
-include L4_IO/fat/subdir.mk
-include L3_Utils/tlm/src/subdir.mk
-include L3_Utils/src/subdir.mk
-include L2_Drivers/src/subdir.mk
-include L2_Drivers/base/subdir.mk
-include L1_FreeRTOS/trace/subdir.mk
-include L1_FreeRTOS/src/subdir.mk
-include L1_FreeRTOS/portable/no_mpu/subdir.mk
-include L1_FreeRTOS/portable/subdir.mk
-include L1_FreeRTOS/hooks/subdir.mk
-include L1_FreeRTOS/MemMang/subdir.mk
-include L0_LowLevel/source/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
lpc1758_freertos_RC.hex \

SECONDARY_LIST += \
lpc1758_freertos_RC.lst \

SECONDARY_SIZE += \
lpc1758_freertos_RC.siz \


# All Target
all: pre-build main-build

# Main-build Target
main-build: lpc1758_freertos_RC.elf secondary-outputs

# Tool invocations
lpc1758_freertos_RC.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -T ../loader.ld -nostartfiles -Xlinker --gc-sections -Wl,-Map,"lpc1758_freertos_RC.map" -specs=nano.specs -o "lpc1758_freertos_RC.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

lpc1758_freertos_RC.hex: lpc1758_freertos_RC.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "lpc1758_freertos_RC.elf"  "lpc1758_freertos_RC.hex"
	@echo 'Finished building: $@'
	@echo ' '

lpc1758_freertos_RC.lst: lpc1758_freertos_RC.elf
	@echo 'Invoking: Cross ARM GNU Create Listing'
	arm-none-eabi-objdump --source --all-headers --demangle --line-numbers --wide "lpc1758_freertos_RC.elf" > "lpc1758_freertos_RC.lst"
	@echo 'Finished building: $@'
	@echo ' '

lpc1758_freertos_RC.siz: lpc1758_freertos_RC.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "lpc1758_freertos_RC.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS)$(CPP_DEPS) lpc1758_freertos_RC.elf
	-@echo ' '

pre-build:
	-sh ../pre_build.sh
	-@echo ' '

post-build:
	-@echo 'Create binary file'
	-arm-none-eabi-objcopy -O binary lpc1758_freertos_RC.elf lpc1758_freertos_RC.bin
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY: main-build pre-build post-build

-include ../makefile.targets
