all:
	$(CC) -o tinync tinync.c -g -lm

cr:
	$(CC) -o coroutine coroutine.c -g -lm

test:
	./coroutine

1000:
	number=1 ; while [ $$number -le 1000 ] ; do\
	       ./m_vecotr; echo $$number ;\
	       number=$$((number + 1)) ;\
	done

massif:
	valgrind --tool=massif ./coroutine


perf:
	sudo perf stat --repeat 10000 -e cache-misses,cache-references,instructions,cycles ./coroutine

address:
	gcc -o memory coroutine.c -g -lm -fsanitize=address

clean_massif:
	rm -rf massif.out.*

clean:
	rm -rf tinync coroutine memory