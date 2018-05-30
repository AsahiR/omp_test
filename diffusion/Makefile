CC = gcc
CFLAGS = -O2 -g -fopenmp
LDFLAGS = -fopenmp
LIBS = 

APP = diffusion_seq
OBJS = $(APP).o

APP_Y = diffusion_y
OBJS_Y = $(APP_Y).o

APP_X = diffusion_x
OBJS_X = $(APP_X).o

all: $(APP) $(APP_X) $(APP_Y)

$(APP): $(OBJS)
	$(CC) $^ $(LIBS) -o $@ $(LDFLAGS)

$(APP_X): $(OBJS_X)
	$(CC) $^ $(LIBS) -o $@ $(LDFLAGS)

$(APP_Y): $(OBJS_Y)
	$(CC) $^ $(LIBS) -o $@ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $*.o

clean:
	rm -f *.o
	rm -f $(APP)
	rm -f $(APP_X)
	rm -f $(APP_Y)
	rm -f *~
