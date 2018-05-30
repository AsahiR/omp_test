CC = gcc
CFLAGS = -O -g -fopenmp
LDFLAGS = -fopenmp
LIBS = 

APP = sort
OBJS = $(APP).o

APP_S = sort_seq
OBJS_S = $(APP_S).o

all: $(APP) $(APP_S)

$(APP): $(OBJS)
	$(CC) $^ $(LIBS) -o $@ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $*.o

clean:
	rm -f *.o
	rm -f *~
	rm -f $(APP)
	rm -f $(APP_S)
