#this  make file is being written to test the papi  library 

all:
	gcc papi_mult.c -o papi_mult  -lpapi
	gcc papi_add.c -o papi_add  -lpapi
run: 
	./papi_test
clean:
	rm -f  *.o *.txt *~ papi_add papi_mult *.out

 
