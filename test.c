#include <stdio.h>
#include <string.h>

#include "../sysprog-audio-1.5/audio.h"
#include "../include/lecteur.h"


int main(int argc, char const *argv[]) {
	printf("Test de la bibliothèque audio\n");
	
	unsigned char filter;
	
	Son s[4];
	
	char names[4][64];
	strcpy(names[0], "test30s8bit1ch.wav");
	strcpy(names[1], "test30s8bit2ch.wav");
	strcpy(names[2], "test30s16bit1ch.wav");
	strcpy(names[3], "test30s16bit2ch.wav");
	
	for(int i = 0;  i < 4; i++) {
		newSon(&s[i], names[i]);
		s[i].write = aud_writeinit (s[i].rate, s[i].size, s[i].channels);
		if(s[i].write < 0) {
			fprintf(stderr,"Error write for %d son", i);
		}
	}
	
	// Test de MONO
	filter = MONO;
	for(int i = 1; i < 4; i++) {
		if(available_filter(&s[i],filter)) {
			// Configure la sortie sur mono
			s[i].write = aud_writeinit (s[i].rate, s[i].size, 1);
			if(s[i].write < 0) {
				fprintf(stderr,"Error write for %d son", i);
			}
			printf("%s Filtre = MONO\n", names[i]);
			read_sound(&s[i],s[i].write, 64, filter, 0);
		}
		else {
			printf("Filter not available\n");
		}
	}
	
	// Test de volume
	filter = VOLUME;
	printf("%d\n",filter);
	for(int i = 1; i < 4; i++) {
		if(available_filter(&s[i],filter)) {
			printf("%s Filtre = VOLUME\n", names[i]);
			s[i].write = aud_writeinit (s[i].rate, s[i].size, s[i].channels);
			if(s[i].write < 0) {
				fprintf(stderr,"Error write for %d son", i);
			}
			read_sound(&s[i],s[i].write, 64, filter, 0.8);
		}
		else {
			printf("Filter not available\n");
		}
	}
	// Test ECHO
	filter = ECHO;
	for(int i = 1; i < 4; i++) {
		if(available_filter(&s[i],filter)) {
			printf("%s Filtre = ECHO\n", names[i]);
			s[i].write = aud_writeinit (s[i].rate, s[i].size, s[i].channels);
			if(s[i].write < 0) {
				fprintf(stderr,"Error write for %d son", i);
			}
			read_sound(&s[i],s[i].write, 64, filter, 0);
		}
		else {
			printf("Filter not available\n");
		}
	}
	
	// Test de tout les filters en meme temps
	filter = MONO|ECHO|VOLUME;
	printf("%d\n",filter);
	for(int i = 1; i < 4; i++) {
		if(available_filter(&s[i],filter)) {
			printf("%s Filtre = ALL\n", names[i]);
			s[i].write = aud_writeinit (s[i].rate, s[i].size, 1);
			if(s[i].write < 0) {
				fprintf(stderr,"Error write for %d son", i);
			}
			read_sound(&s[i],s[i].write, 64, filter, 0.8);
		}
		else {
			printf("Filter not available\n");
		}
	}
	
	
	return 0;
}