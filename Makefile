# MIT License
#
# Copyright (c) 2018 kudima
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

FLAGS = -std=c99 -g -O0
GTK2_INC = `pkg-config --cflags gtk+-2.0 glib-2.0` 
GTK2_LIB = `pkg-config --libs gtk+-2.0 glib-2.0` -lm
#GTK2_LIB = `pkg-config --libs gtk+-2.0` -lm

miner : field_snapshot.o display_field.o mines_field.o main.o scores_callbacks.o scores_interface.o scores_support.o
	gcc $(FLAGS) -o miner field_snapshot.o scores_callbacks.o scores_interface.o scores_support.o main.o display_field.o mines_field.o $(GTK2_LIB)
main.o : main.c
	gcc $(FLAGS) -c main.c -o main.o $(GTK2_INC)
display_field.o : display_field.c
	gcc $(FLAGS) -c display_field.c -o display_field.o $(GTK2_INC)
mines_field.o : mines_field.c 
	gcc $(FLAGS) -c mines_field.c -o mines_field.o $(GTK2_INC)
field_snapshot.o : field_snapshot.c
	gcc $(FLAGS) -c field_snapshot.c -o field_snapshot.o $(GTK2_INC)
#dialog
scores_callbacks.o : dialog/scores/callbacks.c
	gcc $(FLAGS) $(GTK2_INC) -c dialog/scores/callbacks.c -o scores_callbacks.o
scores_interface.o : dialog/scores/interface.c
	gcc $(FLAGS) $(GTK2_INC) -c dialog/scores/interface.c -o scores_interface.o
scores_support.o : dialog/scores/support.c
	gcc $(FLAGS) $(GTK2_INC) -c dialog/scores/support.c -o scores_support.o
clean : 
	rm -f *.o miner




