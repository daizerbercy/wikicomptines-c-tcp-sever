/* fichiers de la bibliothèque standard */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
/* bibliothèque standard unix */
#include <unistd.h> /* close, read, write */
#include <sys/types.h>
#include <sys/socket.h>
/* spécifique à internet */
#include <arpa/inet.h> /* inet_pton */
/* spécifique aux comptines */
#include "comptine_utils.h"

#define PORT_WCP 4321

void usage(char *nom_prog)
{
	fprintf(stderr, "Usage: %s addr_ipv4\n"
			"client pour WCP (Wikicomptine Protocol)\n"
			"Exemple: %s 208.97.177.124\n", nom_prog, nom_prog);
}

/** Retourne (en cas de succès) le descripteur de fichier d'une socket
 *  TCP/IPv4 connectée au processus écoutant sur port sur la machine d'adresse
 *  addr_ipv4 */
int creer_connecter_sock(char *addr_ipv4, uint16_t port);

/** Lit la liste numérotée des comptines dans le descripteur fd et les affiche
 *  sur le terminal.
 *  retourne : le nombre de comptines disponibles */
uint16_t recevoir_liste_comptines(int fd);

/** Demande à l'utilisateur un nombre entre 0 (compris) et nc (non compris)
 *  et retourne la valeur saisie. */
uint16_t saisir_num_comptine(uint16_t nb_comptines);

/** Écrit l'entier ic dans le fichier de descripteur fd en network byte order */
void envoyer_num_comptine(int fd, uint16_t nc);

/** Affiche la comptine arrivant dans fd sur le terminal */
void afficher_comptine(int fd);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}

    int sock = creer_connecter_sock(argv[1], PORT_WCP);
    printf("Connexion établie avec le serveur %s:%d\n", argv[1], PORT_WCP);
    uint16_t nb = recevoir_liste_comptines(sock);
    uint16_t n = saisir_num_comptine(nb);
    envoyer_num_comptine(sock,n);
    afficher_comptine(sock);
    close(sock);
    
	return 0;
}

int creer_connecter_sock(char *addr_ipv4, uint16_t port)
{
	// Création du socket partie client
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // Création de la structure sockaddr_in pour la liaison
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    if(inet_pton(AF_INET, addr_ipv4, &sa.sin_addr)!=1){
        fprintf(stderr, "adresse ipv4 non valable\n");
        exit(1);
    }
    socklen_t sl = sizeof(sa);

    // connection côté client
    if(connect(sock, (struct sockaddr *) &sa, sl) < 0){
        perror("connect");
        exit(EXIT_FAILURE);
    }

    return sock;
}

uint16_t recevoir_liste_comptines(int fd)
{
    char buf[256];
    uint16_t i=0;
    int n=0;
    while((n=read_until_nl(fd,buf)) !=0){
        buf[n+1] = '\0';
        printf("%s",buf);
        i++;
    }

	return i;
}

uint16_t saisir_num_comptine(uint16_t nb_comptines)
{   
    uint16_t n;
	do{
        printf("Quelle comptine voulez-vous ?(Entrer un entier entre 0 et %d) : ",nb_comptines - 1);
        scanf("%hu",&n);
    } while(n>=nb_comptines);

    return n;
}

void envoyer_num_comptine(int fd, uint16_t nc)
{
	uint16_t n = htons(nc);
    write(fd,&n,sizeof(n));
}

void afficher_comptine(int fd)
{
	char buff[256];
    int n;
    while((n=read(fd,buff,sizeof(buff)))>0){
        for(int i=0; i<n; i++){
            if(buff[i] == '\n'){
                printf("\n");
            }else {
                printf("%c", buff[i]);
            }
        }
    }

    if(n<0){
        perror("Erreur lecture du descriteur de fichier");
    }
}
