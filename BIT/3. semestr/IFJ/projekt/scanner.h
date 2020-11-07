/**
 * @file scanner.h
 * @author Simeon Borko, Zdenek Brhel
 * @date 11-12 2017
 * @brief Soubor obsahujujici definice vyctu, struktur a hlavicky funkci algoritmu lexikalne analyzy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "errors.h"
#include "garbage.h"

#ifndef IFJ_SCANNER_H
#define IFJ_SCANNER_H

#define BUFF_EXTEND 10 /**< Implicitni velikost bufferu a jeho rozsireni */

/**
 * @brief Vycet typu tokenu
 * Vycet vsech moznych typu tokenu
 */
typedef enum {
    T_EOL,
    T_ID,
    T_KEYWORD,
    T_INTEGER,
    T_DOUBLE,
    T_STRING,
    T_SEMICOLON,
    T_BR_LEFT,
    T_BR_RIGHT,
    T_COMMA,
    T_PLUS,
    T_MINUS,
    T_MULTIPLIER,
    T_SLASH,
    T_BACKSLASH,
    T_EQUAL,
    T_DIFF,
    T_LESS_THAN,
    T_GREATER_THAN,
    T_LESS_EQUAL,
    T_GREATER_EQUAL
} token_type_t;

/**
 * @brief Vycet stavu
 * Vycet vsech moznych stavu konecneho automatu,
 * ktery slouzi na rozdeleni jednotlivych lexem 
 * na urcity typ tokenu
 */
typedef enum {
    START,
    EOL,

    ID,

    NUMB,
    NUMB_DOT,
    DOUB,
    DOUB_E,
    DOUB_ES,
    DOUBL,

    EXCLAM,
    STR_INSIDE,
    STR_ESCAPE,
    STR_ESCAPE_DEC_1,
    STR_ESCAPE_DEC_2,
    STR_DONE,

    COMM_LINE,
    COMM_BLOCK_INSIDE,
    COMM_BLOCK_APOST,

    SEMICOLON,
    BRACKET_LEFT,
    BRACKET_RIGHT,
    COMMA,
    OP_PLUS,
    OP_MINUS,
    OP_MULTIPLIER,
    OP_SLASH,
    OP_BACKSLASH,
    OP_EQUAL,
    OP_DIFF,
    OP_LESS_THAN,
    OP_GREATER_THAN,
    OP_LESS_EQUAL,
    OP_GREATER_EQUAL
} state_t;

/**
 * @brief Definice zasobniku
 * Zasobnik reprezentuje pole symbolu, ktere se pozdeji meni na tokeny
 */
typedef struct
{
    unsigned size;      /**< Aktualni pocet prvku v zasobniku */
    unsigned capacity;  /**< Maximalni pocet prvku v zasobniku */
    char *string;       /**< Obsah zasobniku */
} buffer_t;

/**
 * @brief Definice tokenu
 * Tokeny reprezentuji jednotlive lexemy
 */
typedef struct
{
    token_type_t type;  /**< Typ tokenu, nabyva hodnot z vyctu token_type_t */
    char *value;        /**< Obsah tokenu */
} token_t;


/* Buffer */

/**
 * Vytvori a inicializuje buffer.
 *
 * @return Ukazatel na nove vytvoreny buffer.
 */
buffer_t *buffer_init();

/**
 * Připojí písmenko na konec bufferu a pokud je malý, tak ho rozšíří.
 *
 * @param buffer Ukazatel na strukturu buffer_t
 * @param c Znak, který má být přidán na konec bufferu
 * @return Vrací 'true' pokud není žádná chyba, 'false' pokud je chyba alokace paměti
 */
bool buffer_append(buffer_t *buffer, char c);

/**
 * Bezpecne smaze buffer z pameti.
 *
 * @param buffer Ukazatel na buffer
 */
//void buffer_destroy(buffer_t *buffer);

/* Token */

/**
 * Vytvori a inicializuje token na zaklade bufferu a stavu automatu.
 *
 * @param buffer Ukazatel na buffer
 * @param state Aktualni stav automatu
 * @return Ukazatel na nove vytvoreny token
 */
token_t *token_init(buffer_t *buffer, state_t state);

/**
 * Zjisti typ tokenu. Funkce predpoklada finalni stav.
 *
 * @param type Ukazatel na vystupny typ
 * @param buffer Ukazatel na buffer
 * @param state Finalni stav automatu
 * @return 'false' kdyz neni mozne zjistit typ (interni chyba), jinak 'true'
 */
bool scanner_token_type(token_type_t *type, buffer_t *buffer, state_t state);

/**
 * Zjistuje, zda obsah bufferu je klicove slovo. Ocekava, ze obsah bufferu je prevedeny na male znaky.
 *
 * @param buffer Ukazatel na buffer
 * @return 'true' kdyz obsah bufferu je nejaky keyword, jinak 'false'
 */
bool scanner_keyword(buffer_t *buffer);

/**
 * Ziska dalsi token ze vstupniho souboru.
 * Nastavi token_ptr na novy token. Pokud se doslo na konec souboru, token je nastaven na NULL.
 *
 * @param token_ptr Adresa na ukazatel tokenu
 * @param buffer Ukazatel na buffer
 * @return E_OK - zadna chyba | E_LEXICAL - lexikalni chyba | E_INTERNAL - interni chyba
 */
error_t token_get(token_t **token_ptr, buffer_t *buffer);

/**
 * Bezpecne smaze token.
 *
 * @param token Ukazatel na token.
 */
//void token_destroy(token_t *token);


/* Zpracovavani znaku */

/**
 * Zpracuje jeden znak.
 * Pridava do bufferu, vytvari tokeny.
 *
 * @param state Ukazatel na stav automatu
 * @param buffer Ukazatel na buffer
 * @param ch Znak na zpracovani
 * @param token_ptr Adresa ukazatele na token
 * @param internal_error Ukazatel na priznak interni chyby
 * @return 'true' pokud byl znak pridan do bufferu nebo byl vytvoren novy token, 'false' pokud nastala lexikalni chyba
 */
bool scanner_process_char(state_t *state, buffer_t *buffer, char ch, token_t **token_ptr, bool *internal_error);

/**
 * Implementace automatu.
 * Pokud muze, prida znak do bufferu.
 * Na zaklade stavu a znaku meni stav.
 *
 * @param state Ukazatel na stav automatu
 * @param buffer Ukazatel na buffer
 * @param c Znak na zpracovani
 * @param internal_error Ukazatel na priznak interni chyby
 * @return 'true' kdyz byl znak pridan do bufferu, jinak 'false'
 */
bool scanner_add(state_t *state, buffer_t *buffer, char c, bool *internal_error);

/**
 * Overi, zda znak muze byt na zacatku identifikatoru.
 *
 * @param c Znak
 * @return 'true' pokud znak muze byt na zacatku identifikatoru, jinak 'false'
 */
bool scanner_can_start_id(char c);

/**
 * Overi, zda znak muze byt jinde nez na zacatku identifikatoru. (Uvnitr nebo na konci.)
 *
 * @param c Znak
 * @return 'true' pokud znak muze byt jinde nez na zacatku identifikatoru, jinak 'false'
 */
bool scanner_can_be_in_id(char c);

/**
 * Rozhodne, zda stav je konecny, nebo nikoliv.
 *
 * @param state Stav
 * @return 'true' pokud je stav konecny, jinak 'false'
 */
bool scanner_is_final_state(state_t state);


bool token_to_int(token_t *token, int **dst);
bool token_to_double(token_t *token, double **dst);
bool token_to_string(token_t *token, char **dst);

#endif // IFJ_SCANNER_H

/* Konec souboru scanner.h */
