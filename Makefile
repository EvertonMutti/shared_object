CC = gcc
CFLAGS = -fPIC -shared -o

buildso:
	$(CC) $(CFLAGS) libcsv.so dependencies/csv_processor.c -I/usr/include/python3.12 -lpython3.12

install-formatters:
	pip install yapf isort ruff

format:
	@yapf ./dependencies -i --recursive
	@isort ./dependencies
	@ruff check ./dependencies --fix

buildtests:
	$(CC) -o test_libcsv test_libcsv.c -L. -lcsv unity/src/unity.c -Iunity/src

clean:
	rm -f libcsv.so
