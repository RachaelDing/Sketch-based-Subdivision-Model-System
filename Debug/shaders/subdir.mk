################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
FRAG_SRCS += \
../shaders/main1.frag 

VERT_SRCS += \
../shaders/main1.vert 

VERT_DEPS += \
./shaders/main1.d 

OBJS += \
./shaders/main1.o 

FRAG_DEPS += \
./shaders/main1.d 


# Each subdirectory must supply rules for building sources it contributes
shaders/%.o: ../shaders/%.frag
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/ugd/xi.wang1/Downloads/boilerplate-updated/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

shaders/%.o: ../shaders/%.vert
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/ugd/xi.wang1/Downloads/boilerplate-updated/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


