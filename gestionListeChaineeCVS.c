//#########################################################
//#
//# Titre : 	Utilitaires Liste Chainee et CVS LINUX Automne 11
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
//# Recherche un item dans la liste chaînée
//# ENTREE: Numéro de la ligne
//# RETOUR:	Un pointeur vers l'item recherché		
//# 		Retourne NULL dans le cas où l'item
//#			est introuvable
//#
struct noeud * findItem(const int no){
	//La liste est vide 
	if ((head==NULL)&&(queue==NULL)) return NULL;

	//Pointeur de navigation
	struct noeud * ptr = head;

	if(ptr->ligne.noligne==no) // premier noeud
		return ptr;
	//Tant qu'un item suivant existe
	while (ptr->suivant!=NULL){

		//Déplacement du pointeur de navigation
		ptr=ptr->suivant;

		//Est-ce l'item recherché?
		if (ptr->ligne.noligne==no){
			//On retourne un pointeur sur l'item (toujours vérrouillé)
			return ptr;
			}
		}
	//On retourne un pointeur NULL
	return NULL;
	}

//#######################################
//#
//# Recherche le PRÉDÉCESSEUR d'un item dans la liste chaînée
//# ENTREE: Numéro de la ligne a supprimer
//# RETOUR:	Le pointeur vers le prédécesseur est retourné		
//# 		Retourne NULL dans le cas où l'item est introuvable
//#
struct noeud * findPrev(const int no){
	//La liste est vide 
	if ((head==NULL)&&(queue==NULL)) return NULL;
	//Pointeur de navigation
	struct noeud * ptr = head;
	//Tant qu'un item suivant existe
	while (ptr->suivant!=NULL){

		//Est-ce le prédécesseur de l'item recherché?
		if (ptr->suivant->ligne.noligne==no){
			//On retourne un pointeur sur l'item précédent
			return ptr;
		}
		//Déplacement du pointeur de navigation
		ptr=ptr->suivant;
		}
	//On retourne un pointeur NULL
	return NULL;
	}

//#######################################
//# Ajoute un item dans la liste chaînée
//# ENTREE: nl: numéro de ligne 
//#			tl: ligne de code 
void addItem(const int nl, const char* tl){
	//Création de l'enregistrement en mémoire
	struct noeud* ni = (struct noeud*)malloc(sizeof(struct noeud));
	struct noeud* ptrINS = findItem(nl);
	//Affectation des valeurs des champs
	ni->ligne.noligne	= nl;
	strcpy(ni->ligne.ptrligne, tl);

	if((ptrINS == NULL) && (nl == 1)) // ajout au debut de la liste vide
	{// premiere ligne premier noeud 
		ni->suivant= NULL;
		queue = head = ni;
	}
	else if ((ptrINS == NULL) && (nl > 1)) // ajout a la fin de la liste
	{
		struct noeud* tptr = queue;
		ni->suivant= NULL;
		queue = ni;
		tptr->suivant = ni;
	}
	else
	{
		struct noeud* tptr = ptrINS;
		if(tptr == head) // ajou a la tete de la liste
			head = ni;
		else
		{
			struct noeud* ptrPREV = findPrev(nl);
			ptrPREV->suivant = ni;
		}
		ni->suivant = tptr;
		
		while (tptr!=NULL){

		//Est-ce le prédécesseur de l'item recherché?
			tptr->ligne.noligne++;
			//On retourne un pointeur sur l'item précédent
			
		//Déplacement du pointeur de navigation
			tptr=tptr->suivant;
		}
	}

}

//#######################################
//# Retire un item de la liste chaînée
//# ENTREE: nl: numéro de ligne 
void removeItem(const int noline){
	struct noeud * ptr;
	struct noeud * tptr;
	struct noeud * optr;
	//Vérification sommaire (no>0 et liste non vide)	
	if ((noline<1)||((head==NULL)&&(queue==NULL)))
		return;

	//Pointeur de recherche
	if(noline==1){
		ptr = head; // suppression du premier element de la liste
	}
	else{
		ptr = findPrev(noline);
	}
	//L'item a été trouvé
	if (ptr!=NULL){

		// Memorisation du pointeur de l'item en cours de suppression
		// Ajustement des pointeurs
		if((head == ptr) && (noline==1)) // suppression de l'element de tete
		{
			if(head==queue) // un seul element dans la liste
			{
				free(ptr);
				queue = head = NULL;
				return;
			}
			tptr = ptr->suivant;
			head = tptr;
			free(ptr);
		}
		else if (queue==ptr->suivant) // suppression de l'element de queue
		{
			queue=ptr;
			free(ptr->suivant);
			ptr->suivant=NULL;
			return;
		}
		else // suppression d'un element dans la liste
		{
			optr = ptr->suivant;	
			ptr->suivant = ptr->suivant->suivant;
			tptr = ptr->suivant;
			free(optr);
		}
		
		
		while (tptr!=NULL){ // ajustement des numeros de ligne

		//Est-ce le prédécesseur de l'item recherché?
			tptr->ligne.noligne--;
			//On retourne un pointeur sur l'item précédent
			
		

		//Déplacement du pointeur de navigation
			tptr=tptr->suivant;
		}
	}
}

//#######################################
//# Modifie un item de la liste chaînée
//# ENTREE: noline: numéro de ligne 
//#			tline: ligne de code 
void modifyItem(const int noline, const char* tline){
	//Vérification sommaire (no>0 et liste non vide)
	if ((noline<1)||((head==NULL)&&(queue==NULL)))
		return;
	//Recherche de l'élément à modifier
	struct noeud * ptr = findItem(noline);
	//L'élément à été trouvé
	if (ptr!=NULL){
		//Modification des champs de l'élément trouvé
		strcpy(ptr->ligne.ptrligne, tline);
	}
}

//#######################################
//#
//# Affiche les items dont le numéro séquentiel est compris dans une plage
//#
void listItems(const int start, const int end){

	//Affichage des entêtes de colonnes
	printf("noligne  texte                                          \n");
	printf("======= ================================================\n");

	struct noeud * ptr = head;			//premier element


	while (ptr!=NULL){

		//L'item a un numéro séquentiel dans l'interval défini
		if ((ptr->ligne.noligne>=start)&&(ptr->ligne.noligne<=end)){
			printf("%d \t %s\n",
				ptr->ligne.noligne,
				ptr->ligne.ptrligne);
			}
		if (ptr->ligne.noligne>end){
			//L'ensemble des items potentiels sont maintenant passés
			//Déplacement immédiatement à la FIN de la liste
			//Notez que le pointeur optr est toujours valide
			ptr=NULL;
			}
		else{
			ptr = ptr->suivant;
		}

	}

	//Affichage des pieds de colonnes
	printf("======= ===============================================\n\n");
	}

