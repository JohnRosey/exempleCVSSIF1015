//#########################################################
//#
//# Titre : 	Utilitaires CVS LINUX Automne 11
//#				SIF-1015 - Système d'exploitation
//#				Université du Québec à Trois-Rivières
//#
//# Auteur : 	Francois Meunier
//#	Date :		Septembre 2011
//#
//# Langage : 	ANSI C on LINUX 
//#
//#######################################

#include "gestionListeChaineeCVS.h"

//Pointeur de tête de liste
extern struct noeud* head;
//Pointeur de queue de liste pour ajout rapide
extern struct noeud* queue;

//#######################################
//#
//# Affiche une série de retour de ligne pour "nettoyer" la console
//#
void cls(void){
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	}

//#######################################
//#
//# Affiche un messgae et quitte le programme
//#
void error(const int exitcode, const char * message){
	printf("\n-------------------------\n%s\n",message);
	exit(exitcode);
	}

//#######################################
//#
//# Chargement de la BD binaire
//#
void loadFich(const char* sourcefname){
	//Initialisation des pointeurs
	head = NULL;
	queue = NULL;


	const char tligne[100];
	int noligne=0;

	//Ouverture du fichier source en mode "rt" : [r]ead [t]ext
	FILE *f;
	f = fopen(sourcefname, "rt");
	if (f==NULL)
		return;

	//Ajout des éléments extraits du fichier source
	
	while(!feof(f)){
		fgets(tligne, 100, f);
		addItem(noligne++, tligne);
		}

 	}

//#######################################
//#
//# Enregistre le contenu de la liste chaînée dans un fichier texte
//#
void saveItems(const char* sourcefname){

	struct noeud * ptr = head;	// premier element
	FILE *f;
	char nomC[100];
	char nomTXT[100];
	char command[100];
	char *ptrC;

	strcpy(nomC,sourcefname);
	strcpy(nomTXT, sourcefname);
	ptrC = strrchr(nomTXT,'.');
	strcpy(ptrC,".txt");
	ptrC = strrchr(nomC,'.');
	strcpy(ptrC,".c");

	//Ouverture du fichier en mode "wt" : [w]rite [t]ext

 	f= fopen(nomTXT, "wt");

	
	if (f==NULL)
		error(2, "saveItems: Erreur lors de l'ouverture du fichier pour écriture en mode texte.");

	while (ptr!=NULL){

		//Écriture des données
		fprintf(f,"%s\n",ptr->ligne.ptrligne);
		
		//Déplacement du pointeur
		ptr = ptr->suivant;
		}
	
	//Fermeture du fichier
	fclose(f);

	// Passage de .txt a .c
	sprintf(command,"mv %s %s",nomTXT,nomC);
	system(command);
	
	}

//#######################################
//#
//# Execute le fichier source .c 
//#
void executeFile(const char* sourcefname){


	char command[100];
	char nameC[100];
	char name[100];


	FILE *f;

	//Ouverture du fichier MakeCVS en mode "wt" : [w]rite [t]ext
	f = fopen("MakeCVS", "wt");
	if (f==NULL)
		error(2, "ExecuteFile: Erreur lors de l'ouverture du fichier pour écriture en mode texte.");

	
	strcpy(nameC,sourcefname);

	fprintf(f,"fichCVSEXE: %s\n",sourcefname);
	fprintf(f,"\tgcc -o fichCVSEXE %s\n",sourcefname);

	//Fermeture du fichier
	fclose(f);

	// make du fichier MakeCVS
	sprintf(command, "make -f MakeCVS");
	system(command);

	// execution du fichier fichCVSEXE
	sprintf(command, "./fichCVSEXE");
	system(command);
	


}


//#######################################
//#
//# fonction utilisée par les threads de transactions
//#
void* readTrans(char* nomFichier){
	FILE *f;
	char buffer[100];
	char *tok, *sp;

	//Ouverture du fichier en mode "r" (equiv. "rt") : [r]ead [t]ext
	f = fopen(nomFichier, "rt");
	if (f==NULL)
		error(2, "readTrans: Erreur lors de l'ouverture du fichier.");

	//Lecture (tentative) d'une ligne de texte
	fgets(buffer, 100, f);

	//Pour chacune des lignes lues
	while(!feof(f)){

		//Extraction du type de transaction
		tok = strtok_r(buffer, " ", &sp);

		//Branchement selon le type de transaction
		switch(tok[0]){
			case 'A':
			case 'a':{
				//Extraction des paramètres
				int noligne = atoi(strtok_r(NULL, " ", &sp));
				char *tligne = strtok_r(NULL, "\n", &sp);
				//Appel de la fonction associée
				addItem(noligne, tligne);
				break;
				}
			case 'E':
			case 'e':{
				//Extraction du paramètre
				int noligne = atoi(strtok_r(NULL, " ", &sp));
				//Appel de la fonction associée
				removeItem(noligne);
				break;
				}
			case 'M':
			case 'm':{
				//Extraction des paramètres
				int noligne = atoi(strtok_r(NULL, " ", &sp));
				char *tligne = strtok_r(NULL, "\n", &sp);
				//Appel de la fonction associée
				modifyItem(noligne,tligne);
				break;
				}
			case 'L':
			case 'l':{
				//Extraction des paramètres
				int nstart = atoi(strtok_r(NULL, "-", &sp));
				int nend = atoi(strtok_r(NULL, " ", &sp));
				//Appel de la fonction associée
				listItems(nstart, nend);
				break;
				}
			case 'S':
			case 's':{
				//Appel de la fonction associée
				char *nomfich = strtok_r(NULL, " ", &sp);
				saveItems(nomfich);
				break;
				}
			case 'O':
			case 'o':{
				//Appel de la fonction associée
				char *nomfich = strtok_r(NULL, " ", &sp);
				loadFich(nomfich);
				break;
				}
			case 'X':
			case 'x':{
				//Appel de la fonction associée
				char *nomfich = strtok_r(NULL, " ", &sp);
				executeFile(nomfich);
				break;
				}
		}

		//Lecture (tentative) de la prochaine ligne de texte
		fgets(buffer, 100, f);
	}
	//Fermeture du fichier
	fclose(f);
	//Retour
	return NULL;
}


