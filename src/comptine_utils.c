#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "comptine_utils.h"

int read_until_nl(int fd, char *buf) //bon
{
	int i=0;

	while((read(fd,buf + i,1) == 1) && (buf[i]!='\n')){
		i++;
	}
	
	return i;
}

int est_nom_fichier_comptine(char *nom_fich)// bon
{
	char cpt[4];
	int size = strlen(nom_fich),j=0,i;
    
    for(i=size-4;i<size+1;i++){
        cpt[j++] = nom_fich[i];
    }
    
    if(strcmp(cpt,".cpt") == 0){
		return 1;
    }
	return 0;
}

struct comptine *init_cpt_depuis_fichier(const char *dir_name, const char *base_name) //bon
{
//chemin ~/dir_name/file
	char chemin[256];
	strcpy(chemin,dir_name);
	strcat(chemin,"/");
	strcat(chemin,base_name);
	// open
	int fd;
	if((fd=open(chemin,O_RDONLY))==-1){
		perror("Open");
		return NULL;
	}
	struct comptine *cpt=malloc(sizeof(struct comptine *));
	if (cpt == NULL) {
        perror("malloc");
        close(fd);
        return NULL;
    }
	//nom_fichier
	cpt->nom_fichier = malloc(sizeof(char *) *(strlen(base_name) + 1));
	if (cpt->nom_fichier == NULL) {
        perror("malloc");
        close(fd);
        free(cpt);
        return NULL;
    }
	strcpy(cpt->nom_fichier,base_name);
	//pour le titre
	char buf[256];
	int n = read_until_nl(fd,buf);
	if(n == -1){
		perror("read_until_nl");
		close(fd);
		return NULL;
	}
	buf[n] = '\0'; //problème was here now is good
	cpt->titre = malloc(sizeof(char *) * (n+2));
	 if (cpt->titre == NULL) {
        perror("malloc");
        close(fd);
		free(cpt->nom_fichier);
        free(cpt);
        return NULL;
    }
	strcpy(cpt->titre,buf);
	
	close(fd);
	return cpt;
}

void liberer_comptine(struct comptine *cpt)
{	
	free(cpt->nom_fichier);
	free(cpt->titre);
	free(cpt);
}


struct catalogue *creer_catalogue(const char *dir_name) 
{
    //open
	DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        perror("opendir");
        return NULL;
    }
	//taille cat->nb
	struct dirent* ent;
	int i = 0;
	while ((ent = readdir(dir)))
		if (est_nom_fichier_comptine(ent->d_name))
			i++;
	// retourne au debut du repertoire
	rewinddir(dir);

	//initialisation avec les bonnes tailles
	struct catalogue* cat =malloc(sizeof(struct catalogue * ));
	if(cat == NULL){
		perror("malloc");
		closedir(dir);
		return NULL;
	}
	
	cat->nb = i;
	cat->tab = malloc(sizeof(struct comptine *) * i);
	if(cat->tab == NULL){
		perror("malloc");
		closedir(dir);
		free(cat);
		return NULL;
	}
	// on remplit le tableau qui est de struct comptine titre et nom du fichier
	i = 0;
	while ((ent = readdir(dir))) {
		if (est_nom_fichier_comptine(ent->d_name)) {
			cat->tab[i] = init_cpt_depuis_fichier(dir_name, ent->d_name);
			// cas d'erreur
			if(cat->tab[i] == NULL){
				perror("NULL");
				liberer_catalogue(cat);
				closedir(dir);
				return NULL;
			}
			i++;
		}
	}
		closedir(dir);
		
	return cat;
}

void liberer_catalogue(struct catalogue *c) //bon
{
	for(int i=0; i<c->nb; i++){
		free(c->tab[i]->nom_fichier);
		free(c->tab[i]->titre);
		free(c->tab[i]);
	}

	free(c->tab);
	free(c);

}
