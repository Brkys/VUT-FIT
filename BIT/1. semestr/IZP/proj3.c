/**
 * Kostra programu pro 3. projekt IZP 2015/16
 *
 * Jednoducha shlukova analyza
 * Complete linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/clsrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */
int errno = 0; //Global variable for returning errors
/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    // TODO
    if(c!= NULL)
    {
        c->capacity = cap; //maximum number of objects in cluster
        c->size = 0; //initialize with 0 objects inside
        c->obj = malloc(cap * sizeof(struct obj_t));
    }
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    // TODO
    free(c->obj);
    init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    // TODO
    int objectCount = c->size+1;
    if(c->capacity <= objectCount) //If capacity is not enough for new object, add capacity
    {
        resize_cluster(c, objectCount);
        if(c == NULL)
        {
            fprintf(stderr, "Error - failed to add object to cluster\n");
            errno = -1;
            return;
        }
    }
    c->obj[objectCount - 1] = obj; //Adding of object to cluster
    c->size = objectCount;
    return;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    // TODO
    for(int i = 0; i < c2->size; i++)
    {
        append_cluster(c1, c2->obj[i]); //Using append_cluster and adding objects from c2 to c1
        if(errno == -1)
        {
            return;
        }
    }
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    // TODO
    free(carr[idx].obj); //Removing objects from cluster carr

    for(int i = idx; i < narr; i++)
    {
        carr[i] = carr[i + 1]; //Set next cluster to position of removed cluster
    }
    return (narr - 1);
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    // TODO
    float distance = sqrt(abs((o2->x - o1->x)*(o2->x - o1->x) + (o2->y - o1->y)*(o2->y - o1->y))); //distance computing
    return distance;
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    // TODO
    float result = 0;
    float tmp;
    float tmp2 = 0;
    float first = 1;

    for(int c1it = 0; c1it < c1->size; c1it++)
    {
        for(int c2it = 0; c2it < c2->size; c2it++)
        {
            tmp = obj_distance(&c1->obj[c1it], &c2->obj[c2it]); //Recieving distance of every point from every point
            if(tmp2 < tmp || first) //If distance is bigger than actual biggest distance, rewrite it
            {
                first = 0;
                tmp2 = tmp;
            }
        }
        if(tmp2 > result)
        {
            result = tmp2;
        }
    }
    return result;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    // TODO
    int result1, result2;
    float resDist, tmpDist;
    int first = 1;
    for(int i = 0; i < (narr - 1); i++)
    {
        for(int i2 = i + 1; i2 < narr; i2++)
        {
            tmpDist = cluster_distance(&carr[i], &carr[i2]); //Search for every cluster another cluster
            if(first || tmpDist < resDist)
            {
                if(first) //At least once
                {
                    first = 0;
                }
                result1 = i;
                result2 = i2;
                resDist = tmpDist;
            }
        }
    }
    *c1 = result1;
    *c2 = result2;
    return;
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    // TODO
    struct obj_t tempObj;
    float posX;
    float posY;
    int ID, cnt, counter = 0;

    FILE* stream = fopen(filename, "r"); //"r" - read only
    if(stream == NULL)
    {
        fprintf(stderr, "Error - can't open file\n");
        return -1;
    }
    fscanf(stream, "count=%d\n", &cnt); //read count of objects in file

    *arr = malloc(cnt * sizeof(struct cluster_t));

    for(int i = 0; i < cnt; i++)
    {
        counter++;
        fscanf(stream, "%d %f %f\n", &ID, &posX, &posY); //load 1 line
        init_cluster(&(*arr)[i], 1); //initialize cluster for 1 object
        tempObj.id = ID;
        tempObj.x = posX;
        tempObj.y = posY;
        append_cluster(&(*arr)[i], tempObj); //add tempObj to arr[i]
        if(errno == -1)
        {
            return -1;
        }
    }
    fclose(stream); //closing of file stream
    return counter;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;
    int c1ID;
    int c2ID;
    int clusterLoaded = 0;
    int clusterCount;
    if(argc == 3)
    {
        if(!(*argv[2] >= '0' && *argv[2] <= '9'))
        {
            fprintf(stderr,"Error - [N] must be a number\n");
            exit(-1);
        }
        clusterCount = strtol(argv[2], NULL, 10);
        if(clusterCount <= 0)
        {
            fprintf(stderr, "Error - parameter [N] must be bigger than 0\n");
            return -1;
        }
        clusterLoaded = load_clusters(argv[1], &clusters);
        if(clusterCount > clusterLoaded)
        {
            fprintf(stderr, "Error - parameter [N] is bigger than number of clusters in file\n");
            for(int i = 0; i < clusterLoaded; i++)
            {
                free(clusters[i].obj);
            }
            exit(-1);
        }
        if(errno == -1)
        {
            return -1;
        }
        while(clusterLoaded > clusterCount)
        {
            find_neighbours(clusters, clusterLoaded, &c1ID, &c2ID);
            merge_clusters(&clusters[c1ID], &clusters[c2ID]);
            if(errno == -1)
            {
                return -1;
            }
            remove_cluster(clusters, clusterLoaded, c2ID);
            clusterLoaded--;
        }
        if(errno == -1)
        {
            return -1;
        }
        print_clusters(clusters, clusterLoaded);
    }
    else
    {
        fprintf(stderr, "Error - missing parameters or too many parameters\n./proj3 FILE_NAME [N]\nFILE_NAME - location of file\n[N] - number of clusters\n");
        return -1;
    }
    for(int i = 0; i < clusterCount; i++)
    {
        free(clusters[i].obj);
    }
    free(clusters);
}

