CXX = cl
CXXFLAGS = /nologo /Wall /WX /EHsc /MD /wd4710 /wd4711 
LDFLAGS = 
!IFNDEF BUILD
BUILD = release
!ENDIF
!IF "$(BUILD)" == "debug"
CXXFLAGS = $(CXXFLAGS) /Od /DDEBUG /MDd
LDFLAGS = /DEBUG
!ELSE
CXXFLAGS = $(CXXFLAGS) /O2 /DNDEBUG
!ENDIF

TARGET = winkey.exe
SRCDIR = sources
INCDIR = includes
BUILDDIR = build
OBJDIR = $(BUILDDIR)\obj
DEPDIR = $(BUILDDIR)\deps

all: $(TARGET)

# Create build directories if they don't exist
$(OBJDIR):
	@if not exist "$(OBJDIR)" mkdir "$(OBJDIR)"

$(DEPDIR):
	@if not exist "$(DEPDIR)" mkdir "$(DEPDIR)"

# Build target - will rebuild if any source or header file is newer than target
$(TARGET): $(OBJDIR) $(DEPDIR) $(SRCDIR)\main.cpp $(INCDIR)\tinky.h
	@echo Building $(TARGET)...
	$(CXX) $(CXXFLAGS) /I$(INCDIR) /Fo$(OBJDIR)\ /Fd$(OBJDIR)\ $(SRCDIR)\*.cpp /link $(LDFLAGS) /OUT:$@ advapi32.lib user32.lib

clean:
	-del /Q *.obj *.pdb *.ilk *.timestamp 2>nul
	-if exist "$(BUILDDIR)" rmdir /s /q "$(BUILDDIR)"

fclean: clean
	-del /Q $(TARGET) 2>nul

re: fclean all

.PHONY: all clean fclean re