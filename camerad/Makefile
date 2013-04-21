CC=gcc
CFLAGS=-c -Wall -I/usr/local/include -I/usr/include/ImageMagick
LDFLAGS=-lgphoto2 -lgphoto2_port -lMagickWand
SOURCES=camerad.c camerad_photo.c camerad_thumb.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=camerad

all: $(SOURCES) $(EXECUTABLE)
  
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)	

