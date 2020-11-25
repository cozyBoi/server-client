CFLAGS = -D_REENTRANT -Wall -pedantic -Ilib
LDLIBS = -lpthread

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

TARGETS = src/server src/client \
	libthreadpool.so libthreadpool.a

all: $(TARGETS)

src/server: src/server lib/threadpool.o
src/client: src/client lib/threadpool.o
lib/threadpool.o: lib/threadpool.c lib/threadpool.h
src/server.o: src/server.c lib/threadpool.h
src/client.o: src/client.c lib/threadpool.h

# Short-hand aliases
shared: libthreadpool.so
static: libthreadpool.a

libthreadpool.so: lib/threadpool.c lib/threadpool.h
	$(CC) -shared -fPIC ${CFLAGS} -o $@ $< ${LDLIBS}

lib/libthreadpool.o: lib/threadpool.c lib/threadpool.h
	$(CC) -c -fPIC ${CFLAGS} -o $@ $<

libthreadpool.a: lib/libthreadpool.o
	ar rcs $@ $^

clean:
	rm -f $(TARGETS) *~ */*~ */*.o

