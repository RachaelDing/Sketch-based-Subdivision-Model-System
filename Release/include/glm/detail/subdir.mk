################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../include/glm/detail/glm.cpp 

OBJS += \
./include/glm/detail/glm.o 

CPP_DEPS += \
./include/glm/detail/glm.d 


# Each subdirectory must supply rules for building sources it contributes
include/glm/detail/%.o: ../include/glm/detail/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/ugc/yichen.ding/last_hour/cpsc589_project/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


