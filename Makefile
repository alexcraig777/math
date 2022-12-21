CC = gcc
CFLAGS = -g -Wall -Wextra

# These represent the layers of dependency within the project.
# All files at higher levels depend on all files at lower layers.
layer_1 = real.o
layer_2 = $(layer_1) arithmetic.o
layer_3 = $(layer_2) decimal.o

test_objects = $(foreach obj,$(layer_3),test_$(obj))

product = newton_pi


# The build rule for the final product executable.
$(product): $(layer_3) $(product).o
	$(CC) $(CFLAGS) $^ -o $@

# The build rule for all object files.
$(layer_3) $(test_objects) test.o $(product).o: %.o: %.c
	$(CC) $(CFLAGS) $^ -c -o $@


# Testing
test_elfs = $(foreach test_obj, $(test_objects),$(subst .o,,$(test_obj)))
run_tests = $(foreach test_elf, $(test_elfs),$(subst test_,run_,$(test_elf)))

# The compilation rules for the test executables.
$(test_elfs): %: %.o test.o
	$(CC) $(CFLAGS) $^ -o $@

# The execution rules for all tests.
$(run_tests): run_%: test_%
	valgrind ./$<

# Dependencies for the test executables.
test_real: $(layer_1)

test_arithmetic: $(layer_2)

test_trig: $(layer_3)
test_decimal: $(layer_3)

test_all: clean $(run_tests)


.PHONY: clean $(run_tests)

clean:
	rm -f *~ *.o $(test_elfs) $(product)
