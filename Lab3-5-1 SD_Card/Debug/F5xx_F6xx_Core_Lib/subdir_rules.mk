################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
F5xx_F6xx_Core_Lib/%.obj: ../F5xx_F6xx_Core_Lib/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs900/ccs/tools/compiler/ti-cgt-msp430_18.12.1.LTS/bin/cl430" -vmspx --abi=eabi --code_model=large --data_model=large -g --include_path="C:/ti/ccs900/ccs/ccs_base/msp430/include" --include_path="C:/ti/ccs900/ccs/tools/compiler/ti-cgt-msp430_18.12.1.LTS/include" --advice:power=all --define=__MSP430F5529__ --diag_warning=225 --display_error_number --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="F5xx_F6xx_Core_Lib/$(basename $(<F)).d_raw" --obj_directory="F5xx_F6xx_Core_Lib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


