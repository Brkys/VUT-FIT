/*! \mainpage My Personal Index Page
 *
 * \section intro_sec Úvod
 * 
 * \subsection Dokumentace 3. projektu IZP
 * \subsection FIT VUT Brno
 * \subsection inic_sec ----Autor----
 *
 * \subsection step1 Jméno : Zdeněk Brhel
 * \subsection step2 Třída : BIA
 * \subsection step3 Login : xbrhel04
 *  
 *
 */
 
 /**
 * Kostra hlavickoveho souboru 3. projekt IZP 2015/16
 * a pro dokumentaci Javadoc.
 */
 
/** @defgroup 1.Deklarace_datovych_typu Deklarace datovych typu
 *  Skupina deklarujici datove typy
 *  @{
 */

/**
 * Definice struktury obj_t
 */
struct obj_t {
    int id;
    float x;
    float y;
};

/**
 * Definice struktury cluster_t
 */
struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};
/** @} */

/**
 * @defgroup 2.Deklarace_funkci Deklarace zakladnich funkci
 * Funkce pro praci se shluky
 * @{
 */
 
/**
 * Inicializace shluku, alokace pameti pro shluku
 * @param c Inicializovany shluku
 * @param cap Kapacita shluku
 * @pre c nesmí být NULL
 * @pre cap musí být větší nebo rovno nule
 */ 
void init_cluster(struct cluster_t *c, int cap);


/**
 * Odstraneni objektu ve shluku a jeho inicializace na kapacitu 0
 * @param c Shluk, ve kterem jsou odstraneny objekty
 */
void clear_cluster(struct cluster_t *c);

/**
 * Konstanta definovana pro realokaci
 */
extern const int CLUSTER_CHUNK;

/** 
 * Realokace zhluku na novou kapacitu
 * @param c Meneny shluku
 * @param new_cap Nova kapacita shluku 
 * @pre c musi existovat
 * @pre Kapacita shluku musi byt vetsi nebo rovno 0
 * @pre Nova kapacita shluku musi byt vetsi nebo rovno 0
 * @return Novy shluk
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * Pridani objektu do shluku
 * @param c Shluk, do ktereho se pridavaji objekty
 * @param obj Objekt pridany do shluku
 * @post Vraci slouceny shluk
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * Slouci 2 shluky
 * @param c1 Shluk, do ktereho budou pridany objekty
 * @param c2 Shluk, ze ktereho se budou brat objekty
 * @pre c1 nesmi byt NULL
 * @pre c2 nesmi byt NULL
 * @post Vraci serazenou c1
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/** @} */

/**
 * @defgroup 3.Pole Prace s polem
 * Funkce pracujici s polem shluku
 * @{
 */
 
 /**
  * Odstrani shluk z pole shluku
  * @param carr Pole shluku
  * @param narr Pocet shluku v poli
  * @param idx Pozice shluku urceneho k odstraneni
  * @pre idx musi byt mensi nez narr
  * @pre narr musi byt vetsi nez 0
  * @return Novy pocet shluku v poli
  */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * Vypocet euklidovske vzdalenosti objektu
 * @param o1 Prvni objekt
 * @param o2 Druhy objekt
 * @pre o1 nesmi byt NULL
 * @pre o2 nesmi byt NULL
 * @return Vzdalenost mezi objekty
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * Vypocet vzdalenosti dvou shluku
 * @param c1 Prvni shluk
 * @param c2 Druhy shluk
 * @pre c1 nesmi byt NULL
 * @pre c2 nesmi byt NULL
 * @pre c1->size musi byt vetsi nez 0
 * @pre c2->size musi byt vetsi nez 0
 * @post Nejblizsi c1 pro c2
 * @return Vzdalenost mezi c1 a c2
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * Hleda 2 nejblizsi shluky
 * @param carr Pole shluku
 * @param narr Velikost pole shluku
 * @param c1 Prvni pozice v poli
 * @param c2 Druha pozice v poli
 * @post Vraci nejblizsi pozice c1 a c2
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * Radi objekty ve shluku podle identifikatoru
 * @param c Shluk obsahujici objekty
 * @post Vraci serazeny shluk
 */
void sort_cluster(struct cluster_t *c);

/**
 * Vypise shluk na stdout
 * @param c Vypisovany shluk
 */
void print_cluster(struct cluster_t *c);

/**
 * Nacte shluky do pole shluku
 * @param filename Lokace a jmeno souboru
 * @param arr Pole shluku
 * @pre arr nesmi byt NULL
 * @return Pocet shluku v poli
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * Vypise pole shluku na stdout
 * @param carr Pole shluku
 * @param narr Pocet shluku v poli
 */
void print_clusters(struct cluster_t *carr, int narr);
/** @} */