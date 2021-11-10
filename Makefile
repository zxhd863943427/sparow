CC = gcc
TARGET=test
#TARGET=spr
DIRS = object include cli compiler parser vm gc ./
#CFLAGS = -g -DDEBUG -lm -Wall -I object -I vm -I compiler -I parser -I include -I cli -I gc -W -Wstrict-prototypes -Wmissing-prot otypes -Wsystem-headers
#CFLAGS = -g -lm -Wall -I object -I vm -I compiler -I parser -I include -I cli -I gc -W -Wstrict-prototypes -Wsystem-headers -fgnu89-inline
CFLAGS = -g -lm -Wall -I object -I vm -I compiler -I parser -I include -I cli -I gc -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers
CFILES =$(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
TEMP=$(patsubst %.c,%.o,$(CFILES))
OBJS=$(subst /,\,$(TEMP))

TEST=include\utils.o
$(TARGET):$(OBJS) 
	@echo "$(OBJS)"
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)

clean:
	del $(TARGET) $(OBJS) 

r: clean $(TARGET)