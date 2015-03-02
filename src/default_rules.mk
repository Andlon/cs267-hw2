$(OBJFILES): %.o: %.cpp $(ADDHEADER)
	$(CC) -I../ -c $(CFLAGS) $<  

$(BINFILES): %: %.o $(ADDLIBS)
	$(CC) $(LFLAGS) -o $@ $< $(ADDLIBS)
	-mv $@ ../bin

all: $(BINFILES) $(OBJFILES)

clean:
	-rm -f *.o
	-rm -f $(foreach bin,$(BINFILES),../bin/$(bin))
