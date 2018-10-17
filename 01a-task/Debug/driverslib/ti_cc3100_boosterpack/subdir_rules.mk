################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
driverslib/ti_cc3100_boosterpack/cc3100_boosterpack.obj: C:/Users/Pere\ Tuset/Desktop/Bitbucket/UOC/Sistemas\ Embebidos/2018_pac2/driverslib/ti_cc3100_boosterpack/cc3100_boosterpack.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/ccsv8/ccs_base/arm/include" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/cc3100/simplelink/source" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/ti_msp432_launchpad" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/freertos/inc" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/freertos/cortex-m4" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/cmsis" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/inc" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/msp432" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/cc3100/board" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/cc3100/oslib" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/cc3100/simplelink" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/cc3100/simplelink/include" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/driverslib/ti_cc3100_boosterpack" --include_path="C:/Users/Pere Tuset/Desktop/Bitbucket/UOC/Sistemas Embebidos/2018_pac2/01a-task" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.2.LTS/include" --include_path="C:/ti/ccsv8/ccs_base/arm/include/CMSIS" --advice:power="all" --define=__MSP432P401R__ --define=TARGET_IS_MSP432P4XX --define=ccs -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="driverslib/ti_cc3100_boosterpack/cc3100_boosterpack.d_raw" --obj_directory="driverslib/ti_cc3100_boosterpack" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


