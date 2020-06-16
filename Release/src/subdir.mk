################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Camera.cpp \
../src/Catmull.cpp \
../src/Geometry.cpp \
../src/InputHandler.cpp \
../src/MeshTopology.cpp \
../src/Program.cpp \
../src/RenderEngine.cpp \
../src/ShaderTools.cpp \
../src/main.cpp \
../src/triangle.cpp 

OBJS += \
./src/Camera.o \
./src/Catmull.o \
./src/Geometry.o \
./src/InputHandler.o \
./src/MeshTopology.o \
./src/Program.o \
./src/RenderEngine.o \
./src/ShaderTools.o \
./src/main.o \
./src/triangle.o 

CPP_DEPS += \
./src/Camera.d \
./src/Catmull.d \
./src/Geometry.d \
./src/InputHandler.d \
./src/MeshTopology.d \
./src/Program.d \
./src/RenderEngine.d \
./src/ShaderTools.d \
./src/main.d \
./src/triangle.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I"/home/ugc/yichen.ding/last_hour/cpsc589_project/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


