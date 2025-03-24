all:
	gcc -o graph_gen main.c api_comm.c graph_generator.c graph_matrix.c utils.c -lcurl
	gcc tester.c -o tester

clean:
	rm -f graph_gen tester

tester:
	gcc tester.c -o tester