DEPENDS_DIR := ../.dependencies
SRC_FILES := $(wildcard ../*/*.cpp) ../main.cpp

DEPENDENCY_FILES := $(addsuffix .d, $(basename $(notdir $(SRC_FILES))))
OBJ_FILES := $(addsuffix .o, $(basename $(notdir $(SRC_FILES))))

LDFLAGS := -L"C:/Program Files/MSYS2/mingw64/x86_64-w64-mingw32/lib" -lglfw3dll -static
CXXFLAGS := -std=c++2a -Wall -Wextra -Wpedantic -g

SHADER_ENDINGS := vert frag geom
SHADER_FILES := $(wildcard $(addprefix ../shaders/*/*., $(SHADER_ENDINGS)))
SHADER_RESULT_DIRS := $(dir $(SHADER_FILES))
SHADER_RESULT_NAMES := $(addsuffix .spv, $(subst .,, $(suffix $(SHADER_FILES))))
SHADER_RESULT_FILES := $(join $(SHADER_RESULT_DIRS), $(SHADER_RESULT_NAMES))


.PHONY: clean app dependencies shaders all
all: app dependencies shaders
app: ../libJAVL.a
dependencies: $(DEPENDENCY_FILES)
shaders: $(SHADER_RESULT_FILES)


../libJAVL.a: $(OBJ_FILES)
	@del ../libJAVL.a || rem
	$(info Creating library: $@)
	@ar rvs $@ $^
#-o $@ $(LDFLAGS)

-include $(DEPENDENCY_FILES)

%.o:
	$(info Compiling: $@)
	@g++ $(CXXFLAGS) -c -o $@ $(filter %/$(basename $@).cpp, $(SRC_FILES))

%.d: $(filter %/$(basename $@).cpp, $(SRC_FILES))
	$(info Looking for dependencies: $@)
	@g++ -MM -o $@ $(filter %/$(basename $@).cpp, $(SRC_FILES))

.SECONDEXPANSION:
$(SHADER_RESULT_FILES): $$(wildcard $$(dir $$@)/*.$$(notdir $$(basename $$@)))
	$(info Compiling shader: $(notdir $^))
	@../glslangValidator -V $^ -o $@

clean:
	del *.d
	del *.o