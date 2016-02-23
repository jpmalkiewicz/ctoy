#

DEFS :=
WARNS := -Wall

SRCS :=
SRCS += display.c
SRCS += main.c
SRCS += status.c
SRCS += toy.c
SRCS += tv.c

LIBS := -lncurses -lm

# ------------------------------------------------

OBJS := $(SRCS:%.c=%.o)
DEPS := $(OBJS:%.o=%.d)

INCS :=
OPTS := -O2

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = $(CFLAGS_$(CROSS_COMPILE)) -MD $(INCS) $(OPTS) $(DEFS) $(WARNS)
ASFLAGS = $(ASFLAGS_$(CROSS_COMPILE)) $(INCS) $(DEFS)
LDFLAGS = $(LDFLAGS_$(CROSS_COMPILE))

all: ctoy

ctoy: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): Makefile

clean:
	$(RM) $(OBJS) $(DEPS)


nuke: clean
	$(RM) ctoy

-include $(DEPS)
