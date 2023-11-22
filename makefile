CC=g++
HEADERS=include/
FLAGS=-I$(HEADERS) -fsanitize=address,alignment -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -fPIE -Werror=vla
SOURCE_DIR:=source/
BIN_DIR:=object/
SOURCES:=$(shell find $(SOURCE_DIR) -name "*.cpp")
obj_unpref:=$(patsubst %.cpp,%.o,$(notdir $(SOURCES)))
OBJECT:=$(addprefix $(BIN_DIR)/,$(obj_unpref))
DEP:=$(patsubst %.o,%.o.d,$(OBJECT))
EXECUTABLE=run

$(EXECUTABLE): $(OBJECT) $(BIN_DIR)
	$(CC) $(FLAGS) $(OBJECT) -o $@

-include $(DEP)

$(BIN_DIR)%.o: $(SOURCE_DIR)%.cpp
	make makedirs
	$(CC) $(FLAGS) -MMD -MF $@.d -c -o $@ $<

.PHONY: makedirs clean doxygen

makedirs:
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(OBJECT)
	rm -rf $(DEP)

doxygen:
	doxygen ./doxygen
