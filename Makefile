CLIBS= 		-lm -lglut -lGL -lGLU -lX11 -lGLEW # 2016
CPATHS=		-I/usr/include -L/usr/lib # 2016
CFLAGS=		$(CPATHS)$(CLIBS)


third:		third.cpp
		g++ third.cpp -o third $(CFLAGS)



clean:
		rm *.o
