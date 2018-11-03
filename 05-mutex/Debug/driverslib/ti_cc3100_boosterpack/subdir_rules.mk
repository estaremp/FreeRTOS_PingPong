################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
driverslib/ti_cc3100_boosterpack/cc3100_boosterpack.obj: /Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/ti_cc3100_boosterpack/cc3100_boosterpack.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Applications/ti/ccsv8/ccs_base/arm/include" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/freertos/inc" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/freertos/cortex-m4" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/msp432" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/cc3100/board" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/cc3100/oslib" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/cc3100/simplelink" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/cc3100/simplelink/include" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/cc3100/simplelink/source" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/cmsis" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/inc" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/ti_cc3100_boosterpack" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/driverslib/ti_msp432_launchpad" --include_path="/Users/randa/Desktop/SistemesEncastats/FreeRTOS_PingPong/05-mutex" --include_path="/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/include" --include_path="/Applications/ti/ccsv8/ccs_base/arm/include/CMSIS" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="driverslib/ti_cc3100_boosterpack/$(basename $(<F)).d_raw" --obj_directory="driverslib/ti_cc3100_boosterpack" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


