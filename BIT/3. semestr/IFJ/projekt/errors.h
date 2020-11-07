/**
 * @file errors.h
 * @author Simeon Borko, Vojtech Chadima
 * @date 11-12 2017
 * @brief Soubor obsahujujici definici vyctoveho typu definujici navratovou hodnotu prekladace
 */

#ifndef IFJ_ERRORS_H
#define IFJ_ERRORS_H

/**
 * @brief Vycet chyb
 * Vycet vsech moznych vystupnich chyb
 */
typedef enum {
    E_OK, 					/**< Vse probehlo bez problemu */
    E_LEXICAL,				/**< Chyba v lexikalni analyze */
    E_SYNTACTIC,			/**< Chyba v syntakticke analyze */
    E_SEM_DEF,				/**< Chyba v semantice - nedefinovana funkce/promenna, pokus o redefinici funkce/promenne */
    E_SEM_TYPE_OR_PARAMS,	/**< Chyba v semantice - chybna typova kompatibilita v aritmetickych, retezcových a relacnich vyrazech, spatný pocet parametru u promenne */
    E_SEM_OTHERS = 6,		/**< Chyba v semantice - ostatni semanticke chyby */
    E_INTERNAL = 99			/**< Interni chyba prekladace - chyba alokace pameti, ... */
} error_t;

#endif //IFJ_ERRORS_H
