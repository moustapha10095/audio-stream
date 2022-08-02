all : obj audioserver audioclient test

audioclient : obj/audioclient.o obj/audio.o obj/lecteur.o obj/socket.o
	gcc obj/audioclient.o obj/audio.o obj/lecteur.o obj/socket.o -o  audioclient

audioserver : obj/audioserver.o obj/audio.o obj/lecteur.o obj/socket.o
	gcc obj/audioserver.o obj/audio.o obj/lecteur.o obj/socket.o -o audioserver
	

obj/audioclient.o: src/audioclient.c
	gcc -c src/audioclient.c -o obj/audioclient.o

obj/audioserver.o: src/audioserver.c
	gcc -c src/audioserver.c -o obj/audioserver.o

obj/lecteur.o : src/lecteur.c
	gcc -c src/lecteur.c -o obj/lecteur.o

obj/audio.o : sysprog-audio-1.5/audio.c
	gcc -c sysprog-audio-1.5/audio.c -o obj/audio.o

obj/socket.o : src/socket.c
	gcc -c src/socket.c -o obj/socket.o

obj:
	mkdir -p obj

clean :
	rm -f obj/*.o
	rm -f audioclient audioserver test