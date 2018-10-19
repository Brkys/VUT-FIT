	
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, říjen 2017
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    //-----------------------------------------------------------------------------------------------------------------------------------------------------
	L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	tDLElemPtr gotta_free_em_all;
	//Vytvořím element a postupně uvolňuji a předávám pointer na další element
	while(L->First != NULL)
	{
		gotta_free_em_all = L->First->rptr;
		free(L->First);
		L->First = gotta_free_em_all;
	}

	L->Last = NULL;
	L->Act = NULL;
	
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr newElem;

	if((newElem = malloc(sizeof(struct tDLElem))) == NULL) //alokace
	{
		DLError();
		return;
	}
	newElem->data = val;
	if(L->First == NULL) //pokud se jedná o první prvek
	{
		L->First = L->Last = newElem;
		newElem->lptr = newElem->rptr = NULL;
	}
	else //pokud ne
	{
		L->First->lptr = newElem;
		newElem->rptr = L->First;
		newElem->lptr = NULL;
		L->First = newElem;
	}
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr newElem;

	if((newElem = malloc(sizeof(struct tDLElem))) == NULL) //alokace s ošetřením chyby
	{
		DLError();
		return;
	}

	newElem->data = val;
	if(L->First == NULL)  // Pokud je to první prvek
	{
		L->First = L->Last = newElem;
		newElem->rptr = newElem->lptr = NULL;
	}
	else //pokud ne
	{
		L->Last->rptr = newElem;	
		newElem->lptr = L->Last;
		newElem->rptr = NULL;
		L->Last = newElem;
	}
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if(L->First == NULL)
	{
		DLError();
		return;
	}

	*val = L->First->data;
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if(L->First == NULL)
	{
		DLError();
		return;
	}

	*val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	if(L->First != NULL)
	{
		if(L->First == L->Last)//Pokud obsahuje jen jeden prvek
		{
			L->Act = NULL;
			L->First = NULL;
			L->Act = NULL;
		}
		else
		{
			tDLElemPtr todel = L->First;
			if(L->First == L->Act)
				L->Act = NULL;
			L->First = todel->rptr;
			L->First->lptr = NULL;
			free(todel);
		}	
	}
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	if(L->First != NULL)
	{
		if(L->First == L->Last) //pokud je první zároveň poslední
		{
			L->Act = NULL;
			L->First = NULL;
			L->Act = NULL;
		}
		else
		{
			if(L->Last == L->Act)
				L->Act = NULL;
			L->Last->lptr->rptr = NULL;
			tDLElemPtr elem;
			elem = L->Last;
			L->Last = elem->lptr;
			free(elem);
		}
	}
	
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	
			
	 if(L->Act == NULL || (L->Act == L->Last))
	 {
	 	return;
	 }
	 else
	 {
	 	if(L->Act->rptr != NULL) //Pokud je co rušit
	 	{
	 		tDLElemPtr elem;
	 		elem = L->Act->rptr; //Vytvoření pointru na rušený
	 		L->Act->rptr = elem->rptr; //přepnutí rušeného
	 		if(elem == L->Last)
	 		{
	 			L->Last = L->Act; //poslední se stane aktivní, jestli elem = poslední
	 			free(elem);
	 		}
	 		else
	 		{
	 			elem->rptr->lptr = L->Act;
	 			free(elem);
	 		}
	 	}

	 }
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	
	//Není třeba komentář, je to to samé co v předchozí funkci, až na to, že
	//mažeme předchozí prvek
	if(L->Act == NULL || L->Act == L->First)
		return;
	else
	{
		if(L->Act->lptr != NULL)
		{
			tDLElemPtr elem;
			elem = L->Act->lptr;
			L->Act->lptr = elem->lptr;
			if(elem == L->First)
			{
				L->First = L->Act;
			}
			else
			{
				elem->lptr->rptr = L->Act;
			}
			free(elem);
		}
	}
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	// Pokud nebyl seznam L aktivní, nic se neděje.
	if (L->Act != NULL) {

		tDLElemPtr elem;
		if ((elem = malloc(sizeof(struct tDLElem))) == NULL) {
			DLError();
			return;
		}

		elem->data = val;
		elem->rptr = L->Act->rptr;
		elem->lptr = L->Act;
		L->Act->rptr = elem;
		if (L->Act == L->Last) {
			L->Last = elem;
		} else {
			elem->rptr->lptr = elem;
		}
	}

}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
	// to samé jako minulá funkce
	if (L->Act != NULL) {

		tDLElemPtr elem;
		if ((elem = malloc(sizeof(struct tDLElem))) == NULL) {
			DLError();
			return;
		}

		elem->data = val;
		elem->lptr = L->Act->lptr;
		elem->rptr = L->Act;
		L->Act->lptr = elem;
		if (L->Act == L->First) {
			L->First = elem;
		} else {
			elem->lptr->rptr = elem;
		}
	}
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if(L->Act == NULL) //ošetření act =null
	{
		DLError();
		return;
	}
	*val = L->Act->data;
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if(L->Act != NULL) //ošetření protí null
		L->Act->data = val;
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if(L->Act != NULL) //pokud by byl null, nic se neděje
	{
		if(L->Act != L->Last)
			L->Act = L->Act->rptr;
		else
			L->Act = NULL;
	}
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if(L->Act != NULL) //Pokudj je, opět se nic neděje
	{
		if(L->Act != L->First)
			L->Act = L->Act->lptr;
		else
			L->Act = NULL;
	}
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return L->Act == NULL ? 0 : 1;
}

/* Konec c206.c*/
