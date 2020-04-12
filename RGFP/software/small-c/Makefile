PREFIX = riscv32-unknown-elf-
CXX = $(PREFIX)gcc
CXX_FLAGS = -march=rv32i -mabi=ilp32 -nostartfiles -g

TARGET = $(notdir $(PWD))
$(warning TARGET-> $(TARGET))

ASM_SRCs = $(wildcard *.S)
C_SRCs = $(wildcard *.c)
SRCs = $(ASM_SRCs) $(C_SRCs)
$(warning source file-> $(SRCs))

ASM_OBJs = $(ASM_SRCs:%.S=%.o)
C_OBJs = $(C_SRCs:%.c=%.o)
OBJs = $(ASM_OBJs) $(C_OBJs)
$(warning object file-> $(OBJs))

$(TARGET):$(OBJs)
	$(PREFIX)gcc $(CXX_FLAGS) $(OBJs) -o $(TARGET)
	$(PREFIX)readelf -a $(TARGET) > $(TARGET).readelf
	$(PREFIX)objdump -S -M no-aliases,numeric $(TARGET) > $(TARGET).dump
	objdump -s --section .comment $(TARGET) > $(TARGET).comment

$(OBJs):$(ASM_OBJs) $(C_OBJs)

$(ASM_OBJs):$(ASM_SRCs)
	$(PREFIX)gcc $(CXX_FLAGS) -c $<

$(C_OBJs):$(C_SRCs)
	$(PREFIX)gcc $(CXX_FLAGS) -c $< -o $@


clean:
	-rm -f $(TARGET) $(TARGET).readelf $(TARGET).dump $(TARGET).comment $(OBJs)
