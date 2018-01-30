################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HapLinux.cpp \
../src/HapMdns.cpp \
../src/HapTcp.cpp \
../src/HapTest.cpp 

OBJS += \
./src/HapLinux.o \
./src/HapMdns.o \
./src/HapTcp.o \
./src/HapTest.o 

CPP_DEPS += \
./src/HapLinux.d \
./src/HapMdns.d \
./src/HapTcp.d \
./src/HapTest.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	${CC} -std=c++14 -I../Hap -I../Hap/crypt -I../Hap/srp -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


