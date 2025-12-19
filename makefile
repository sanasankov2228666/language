FLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

INCLUDES = -I./headers

OTHER_SRC = source/graphic_dump.cpp source/tree.cpp source/str_funcs.cpp source/lexems.cpp

SPU_INCLUDES = -I./SPU/headers_spu
STACL_INCLUDES = -I./stack/headers

SPU_FILES = SPU/spu.cpp SPU/str_func_spu.cpp SPU/main_spu.cpp stack/stack1.cpp stack/checker.cpp SPU/fileopen.cpp
ASM_FILES = SPU/assembler/assembler.cpp SPU/spu.cpp SPU/str_func.cpp SPU/assembler/main_assembler.cpp stack/stack1.cpp stack/checker.cpp SPU/fileopen.cpp

FRONTEND_SRC = $(wildcard source/frontend/*.cpp)
FILES_FRONTEND = $(FRONTEND_SRC) $(OTHER_SRC)

BACKEND_SRC = $(wildcard source/backend/*.cpp)
FILES_BACKEND = $(BACKEND_SRC) $(OTHER_SRC)

all: front

front: $(FILES)
	@echo "=== Compiling front ==="
	g++ $(FLAGS) $(FILES_FRONTEND) $(INCLUDES) -o frontend.exe
	@echo "=== Compilation complete ==="

back: $(FILES)
	@echo "=== Compiling back ==="
	g++ $(FLAGS) $(FILES_BACKEND) $(INCLUDES) -o backend.exe
	@echo "=== Compilation complete ==="

spu: $(SPU_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ $(FLAGS) $(SPU_FILES) $(SPU_INCLUDES) -o spu.exe
	@echo "-----------------------------------------------------------------------------------------"

asm: $(ASM_FILES)
	@echo "-----------------------------------------------------------------------------------------"
	g++ $(FLAGS) $(ASM_FILES) $(SPU_INCLUDES) -o assembler.exe
	@echo "-----------------------------------------------------------------------------------------"

run-front: front
	@echo "=== Running front ==="
	./frontend.exe

run-asm: asm
	./assembler.exe

run-spu: spu
	./spu.exe

run-back: back
	@echo "=== Running back ==="
	./backend.exe

clean:
	rm -f frontend.exe
	rm -f backend.exe

help:
	@echo "Available commands:"
	@echo "  make all       - compile front (default)"
	@echo "  make front     - compile front"
	@echo "  make run-front - compile and run front"
	@echo "  make back      - compile and run back"
	@echo "  make run-back  - compile and run back"	
	@echo "  make spu        - compile SPU"
	@echo "  make asm        - compile assembler"
	@echo "  make run-asm    - compile and run assembler"	
	@echo "  make clean     - remove compiled program"
	@echo "  make help      - show this help"