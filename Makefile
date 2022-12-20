CC = gcc
CFLAGS = -g -Wall -Wextra

layer_1 = real.o
layer_2 = $(layer_1) arithmetic.o
layer_3 = $(layer_2) trig.o decimal.o

test_objects = $(foreach obj,$(layer_3),test_$(obj))

tests = $(foreach test_obj, $(test_objects),$(subst .o,,$(test_obj)))

product = newton_pi

$(product): $(layer_3)
	$(CC) $(CFLAGS) $^ -c -o $@

$(layer_3) $(test_objects) test.o $(product).o: %.o: %.c
	$(CC) $(CFLAGS) $^ -c -o $@

$(tests): %: %.o test.o
	$(CC) $(CFLAGS) $^ -o $@
	valgrind ./$@

test_real: $(layer_1)

test_arithmetic: $(layer_2)

test_trig: $(layer_3)
test_decimal: $(layer_3)

.PHONY: clean test

clean:
	rm -f *~ *.o $(tests) $(product)
