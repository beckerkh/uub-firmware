################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/config_trigger.c \
/home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/linux_trigger_test.c \
/home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/map_registers.c \
/home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/muon_buf_utils.c \
/home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/shwr_buf_utils.c \
/home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/trigger_test.c 

OBJS += \
./src/config_trigger.o \
./src/linux_trigger_test.o \
./src/map_registers.o \
./src/muon_buf_utils.o \
./src/shwr_buf_utils.o \
./src/trigger_test.o 

C_DEPS += \
./src/config_trigger.d \
./src/linux_trigger_test.d \
./src/map_registers.d \
./src/muon_buf_utils.d \
./src/shwr_buf_utils.d \
./src/trigger_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/config_trigger.o: /home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/config_trigger.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Linux gcc compiler'
	arm-xilinx-linux-gnueabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/linux_trigger_test.o: /home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/linux_trigger_test.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Linux gcc compiler'
	arm-xilinx-linux-gnueabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/map_registers.o: /home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/map_registers.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Linux gcc compiler'
	arm-xilinx-linux-gnueabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/muon_buf_utils.o: /home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/muon_buf_utils.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Linux gcc compiler'
	arm-xilinx-linux-gnueabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/shwr_buf_utils.o: /home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/shwr_buf_utils.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Linux gcc compiler'
	arm-xilinx-linux-gnueabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/trigger_test.o: /home/augerdn/dfnitz/git_clones/uub-firmware/wp2/linux_test_code/trigger_test/trigger_test.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Linux gcc compiler'
	arm-xilinx-linux-gnueabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


