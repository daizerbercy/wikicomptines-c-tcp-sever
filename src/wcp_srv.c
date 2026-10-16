/* fichiers de la bibliothèque standard */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
/* bibliothèque standard unix */
#include <unistd.h> /* close, read, write */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <errno.h>
/* spécifique à internet */
#include <arpa/inet.h> /* inet_pton */
/* spécifique aux comptines */
#include "comptine_utils.h"

#define PORT_WCP 4321

void usage(char *nom_prog)
{
	fprintf(stderr, "Usage: %s repertoire_comptines\n"
			"serveur pour WCP (Wikicomptine Protocol)\n"
			"Exemple: %s comptines\n", nom_prog, nom_prog);
}
/** Retourne en cas de succès le descripteur de fichier d'une socket d'écoute
 *  attachée au port port et à toutes les adresses locales. */
int creer_configurer_sock_ecoute(uint16_t port);

/** Écrit dans le fichier de desripteur fd la liste des comptines présents dans
 *  le catalogue c comme spécifié par le protocole WCP, c'est-à-dire sous la
 *  forme de plusieurs lignes terminées par '\n' :
 *  chaque ligne commence par le numéro de la comptine (son indice dans le
 *  catalogue) commençant à 0, écrit en décimal, sur 6 caractères
 *  suivi d'un espace
 *  puis du titre de la comptine
 *  une ligne vide termine le message */
void envoyer_liste(int fd, struct catalogue *c);

/** Lit dans fd un entier sur 2 octets écrit en network byte order
 *  retourne : cet entier en boutisme machine. */
uint16_t recevoir_num_comptine(int fd);

/** Écrit dans fd la comptine numéro ic du catalogue c dont le fichier est situé
 *  dans le répertoire dirname comme spécifié par le protocole WCP, c'est-à-dire :
 *  chaque ligne du fichier de comptine est écrite avec son '\n' final, y
 *  compris son titre, deux lignes vides terminent le message */
void envoyer_comptine(int fd, const char *dirname, struct catalogue *c, uint16_t ic);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}
    int sock_srv,sock_clt;
    sock_srv = creer_configurer_sock_ecoute(PORT_WCP);
    
    printf("Serveur en attente de connexions...\n");
    while(1){
        struct sockaddr_in sa_clt;
        socklen_t sl = sizeof(sa_clt);
        sock_clt = accept(sock_srv,(struct sockaddr *) &sa_clt, &sl);
        if(sock_clt <0){
            perror("Erreur lors de la connexion");
            exit(2);
        }

        printf("Connexion acceptée\n");
        struct catalogue * c= creer_catalogue(argv[1]);
        envoyer_liste(sock_clt,c);
        printf("liste envoyée\n");
        uint16_t recp_num_comp = recevoir_num_comptine(sock_clt);

        envoyer_comptine(sock_clt,argv[1],c,recp_num_comp);
        printf("comptine envoyée\n\n");

        close(sock_clt);
        liberer_catalogue(c);
    }
    close(sock_srv);
	return 0;
}
/** Retourne en cas de succès le descripteur de fichier d'une socket d'écoute
* attachée au port port et à toutes les adresses locales. */
int creer_configurer_sock_ecoute(uint16_t port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(2);
    }

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr=htonl(INADDR_ANY);

    if(bind(sock,(struct sockaddr *) &sa, sizeof(sa)) < 0){
        perror("bind");
        exit(3);
    }

    if(listen(sock,128)< 0 ){
        perror("listen");
        exit(2);
    }
    return sock;
}

void envoyer_liste(int fd, struct catalogue *c)
{
    for(int i=0 ;i<c->nb ;i++){
        dprintf(fd, "%6d %s\n",i,c->tab[i]->titre);
    }
	dprintf(fd,"\n");
}

uint16_t recevoir_num_comptine(int fd)
{   
    uint16_t n;
	read(fd,&n,sizeof(uint16_t));
    n = ntohs(n);
	return n;
}

void envoyer_comptine(int fd, const char *dirname, struct catalogue *c, uint16_t ic)
{   // chemin du fichier pour l'ouvrir
    char chemin[256];
	strcpy(chemin,dirname);
	strcat(chemin,"/");
	strcat(chemin,c->tab[ic]->nom_fichier);
    int fd2;
    if((fd2=open(chemin, O_RDONLY)) == -1){
        perror("open");
        return;
    }

    // écriture de la comptine dans fd
    int n;
    char buf[256];
    while((n=read(fd2,buf,sizeof(buf)))>0){
        write(fd,buf,n);
    }

    write(fd,"\n\n",2);
    close(fd2);
}
