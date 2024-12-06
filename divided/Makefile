all: pos
	
pos: main.o table.o etcFunc.o 7_2l7_5l7_6l7_7.o 7_3.o 7_4.o 7_8l7_9.o 7_10l7_13.o 7_12.o orderItem.o paymentUnit.o tableMove.o paymentCancel.o
	gcc -o pos main.o table.o etcFunc.o 7_2l7_5l7_6l7_7.o 7_3.o 7_4.o 7_8l7_9.o 7_10l7_13.o 7_12.o orderItem.o paymentUnit.o tableMove.o paymentCancel.o
main.o: main.c
	gcc -c main.c
table.o: table.c
	gcc -c table.c
etcFunc.o: etcFunc.c
	gcc -c etcFunc.c
7_2l7_5l7_6l7_7.o: 7_2l7_5l7_6l7_7.c
	gcc -c 7_2l7_5l7_6l7_7.c
7_3.o: 7_3.c
	gcc -c 7_3.c
7_4.o: 7_4.c
	gcc -c 7_4.c
7_8l7_9.o: 7_8l7_9.c
	gcc -c 7_8l7_9.c
7_10l7_13.o: 7_10l7_13.c
	gcc -c 7_10l7_13.c
7_12.o: 7_12.c
	gcc -c 7_12.c
orderItem.o: orderItem.c
	gcc -c orderItem.c
paymentUnit.o: paymentUnit.c
	gcc -c paymentUnit.c
tableMove.o: tableMove.c
	gcc -c tableMove.c
paymentCancel.o: paymentCancel.c
	gcc -c paymentCancel.c
clean:
	rm -f *.o
	rm -f pos