LDFLAGS += `pkg-config --libs x11 xrandr`
CFLAGS += `pkg-config --cflags`

HEADERS = \
	src/windowManager.h \
	src/macros.h \
	src/frame.h
SOURCES = \
	src/main.cpp \
	src/windowManager.cpp \
	src/frame.cpp


all: steveWM
	
steveWM: $(SOURCES) $(HEADERS)
	$(CXX) -o $@ $(SOURCES) $(LDFLAGS) 

clean:
	rm $(OBJECTS) steveWM
