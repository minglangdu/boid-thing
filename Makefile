game:
	g++ main.cpp -o main -I include -L lib -l SDL2-2.0.0 -std=c++11
	./main
	rm main