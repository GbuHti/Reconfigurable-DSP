all : main.c bootstrap.S
	riscv32-unknown-elf-gcc main.c bootstrap.S -o main -march=rv32i -mabi=ilp32 -nostartfiles
	riscv32-unknown-elf-readelf -a main > main.readelf
	riscv32-unknown-elf-objdump -D main > main.dump
	objdump -s --section .comment main > main.com
	
clean:
	rm -f main.*
