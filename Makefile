project: m.cpp
	g++ -o simulation.o m.cpp -lpthread

pr: main.cpp
	g++ -o main.o main.cpp -lpthread
	./main.o configuration_file.txt output.txt

little: main.cpp
	g++ -o main.o main.cpp -lpthread
	./main.o ./testcases/inputs/configuration_file_5.txt output.txt

one: main.cpp
	g++ -o main.o main.cpp -lpthread
	./main.o ./testcases/inputs/configuration_file_1.txt output.txt

six: main.cpp
	g++ -o main.o main.cpp -lpthread
	./main.o ./testcases/inputs/configuration_file_6.txt output.txt

seven: main.cpp
	g++ -o main.o main.cpp -lpthread
	./main.o ./testcases/inputs/configuration_file_7.txt output.txt

eight: main.cpp
	g++ -o main.o main.cpp -lpthread
	./main.o ./testcases/inputs/configuration_file_8.txt output.txt

two: main.cpp
	g++ -o main.o main.cpp -lpthread
	./main.o ./testcases/inputs/configuration_file_2.txt output.txt

three: main.cpp
	g++ -o main.o main.cpp -lpthread
	./main.o ./testcases/inputs/configuration_file_3.txt output.txt

four: main.cpp
	g++ -o main.o main.cpp -lpthread
	./main.o ./testcases/inputs/configuration_file_4.txt output.txt

five: main.cpp
	g++ -o main.o main.cpp -lpthread
	./main.o ./testcases/inputs/configuration_file_5.txt output.txt


mf: m.cpp
	g++ -o m.o m.cpp -lpthread
	./m.o ./testcases/inputs/configuration_file_5.txt output.txt

m2: m.cpp
	g++ -o m.o m.cpp -lpthread
	./m.o ./testcases/inputs/configuration_file_2.txt output.txt

m1: m.cpp
	g++ -o m.o m.cpp -lpthread
	./m.o ./testcases/inputs/configuration_file_1.txt output.txt


m3: m.cpp
	g++ -o m.o m.cpp -lpthread
	./m.o ./testcases/inputs/configuration_file_3.txt output.txt



m4: m.cpp
	g++ -o m.o m.cpp -lpthread
	./m.o ./testcases/inputs/configuration_file_4.txt output.txt



m6: m.cpp
	g++ -o m.o m.cpp -lpthread
	./m.o ./testcases/inputs/configuration_file_6.txt output.txt


m7: m.cpp
	g++ -o m.o m.cpp -lpthread
	./m.o ./testcases/inputs/configuration_file_7.txt output.txt


m8: m.cpp
	g++ -o m.o m.cpp -lpthread
	./m.o ./testcases/inputs/configuration_file_8.txt output.txt

sim1:
	./simulation.o ./testcases/inputs/configuration_file_1.txt output.txt

sim2:
	./simulation.o ./testcases/inputs/configuration_file_2.txt output.txt

sim3:
	./simulation.o ./testcases/inputs/configuration_file_3.txt output.txt

sim4:
	./simulation.o ./testcases/inputs/configuration_file_4.txt output.txt

sim5:
	./simulation.o ./testcases/inputs/configuration_file_5.txt output.txt

sim6:
	./simulation.o ./testcases/inputs/configuration_file_6.txt output.txt

sim7:
	./simulation.o ./testcases/inputs/configuration_file_7.txt output.txt

sim8:
	./simulation.o ./testcases/inputs/configuration_file_8.txt output.txt
