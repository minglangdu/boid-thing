game:
	g++ main.cpp -o main -I /opt/homebrew/include -L /opt/homebrew/lib -l SDL2-2.0.0 -std=c++11
	./main
	rm main
