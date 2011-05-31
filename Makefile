
TARGET = 5900Asn3
SRCS = Core.cpp CgShader.cpp CgShaderCel.cpp CgShaderNormal.cpp CgShaderOutline.cpp CgShaderPhong.cpp GlutApplication.cpp RenderTexture.cpp CgShaderCelScreen.cpp

OBJS = $(SRCS:.cpp=.o)

UNAME := $(shell uname)

CXX = g++
CFLAGS += -Wall

ifdef CG_INC_PATH
  CFLAGS += -I"$(CG_INC_PATH)"
endif

ifdef CG_LIB_PATH
  CGLAGS += -L"$(CG_LIB_PATH)"
endif

ifeq ($(UNAME), Darwin)
  CLINKFLAGS += -framework OpenGL -framework GLUT -framework Cg
else
  ifeq ($(findstring CYGWIN, $(UNAME)), CYGWIN)
    CFLAGS += -D_WIN32 -DLGUT_DISABLE_ATEXIT_HACK
    CFLAGS += -I"C:\Program Files\NVIDIA Corporation\Cg\include"	  
    CLINKFLAGS += -L"C:\Program Files\NVIDIA Corporation\Cg\lib"
    CLINKFLAGS += -lcg -lcgGL -lglut32 -lopengl32
    EXE = .exe
  else
	CLINKFLAGS += -L/usr/X11R6/lib64 -L/usr/X11R6/lib
	CLINKFLAGS += -lCg -lCgGL -lglut -lGLU -lGL -lXi -lXmu -lX11 -lm -lpthread
  endif
endif

all: $(TARGET:=$(EXE))

$(TARGET): $(OBJS)
	$(CXX) $(CGLAGS) -o $@ $^ $(CLINKFLAGS)
	
clean:
	$(RM) $(TARGET:=$(EXE)) $(OBJS)

clobber:
	$(RM) *.bak *.o *~ 
	
run:
	./$(TARGET:=$(EXE))
		
debug:
	@echo "Target: " 
	@echo $(TARGET:=$(EXE))
	@echo
	@echo "Sources: " 
	@echo $(SRCS)
	@echo
	@echo "Include paths: " 
	@echo $(CFLAGS)
	@echo
	@echo "Link libs: "
	@echo $(CLINKFLAGS) 

.PHONY: all run clean clobber
	






