all: loglogOn.png speedup.png speedup-16000.png

%.png: %.gnu ../data/*
	gnuplot $< 

clean:
	rm *.png
