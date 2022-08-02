#ifndef LECTEUR_FILE
#define LECTEUR_FILE

#include <unistd.h>
#include <sys/timerfd.h>

#define MONO 0b01
#define VOLUME 0b010
#define ECHO 0b100
#define MAXLINE 1024

typedef struct Son Son;
struct Son {
	int rate;// vitesse
	int size;// longueur
	int channels;// chaine 
	char* name;// nom 
	int read;// lecture 
	int write;// ecriture 
};

void nouveauSon(Son *s, char* name);
void joueSon(Son *s);
void init_echo(unsigned char* a, unsigned char* b);
void echo(Son *s, short* buf,short bufecho[256][MAXLINE], unsigned char* a, unsigned char* b, unsigned short bytes);
void volume(Son *s, char* buf, unsigned short size, float lvl);
void mono(Son *s, char* buf, unsigned short size);
size_t bytes_for_sampling(Son* s);
long elapsed_time_between_sampling(Son* s);
int create_timer(time_t tv_sec, long tv_nsec);
void read_sound(Son *s, int fdout, int multi, unsigned char filter, float lvl);
int available_filter(Son *s, unsigned char filter);

#endif