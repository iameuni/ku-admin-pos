OBJDIR := obj

OBJS := $(OBJDIR)/main.o $(OBJDIR)/table.o $(OBJDIR)/etcFunc.o $(OBJDIR)/7_2l7_5l7_6l7_7.o \
        $(OBJDIR)/7_3.o $(OBJDIR)/7_4.o $(OBJDIR)/7_8l7_9.o $(OBJDIR)/7_10l7_13.o \
        $(OBJDIR)/7_12.o $(OBJDIR)/orderItem.o $(OBJDIR)/paymentUnit.o \
        $(OBJDIR)/tableMove.o $(OBJDIR)/paymentCancel.o

TARGET := pos.exe

# UTF-8 및 한글 출력을 위한 컴파일러 옵션
CFLAGS := -fexec-charset=CP949 -finput-charset=UTF-8

all: $(TARGET)

$(TARGET): $(OBJS)
	gcc $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c
	if not exist $(OBJDIR) mkdir $(OBJDIR)
	gcc $(CFLAGS) -c $< -o $@

clean:
	if exist $(OBJDIR) rmdir /s /q $(OBJDIR)
	if exist $(TARGET) del /q $(TARGET)