/**
 * @file generator.h
 * @author Simeon Borko, Filip Borcik, Vojtech Chadima
 * @date 11-12 2017
 * @brief Soubor obsahujujici hlavicky funkci a definice struktur pro vysledne generovani kodu IFJcode17
 */

#include <stdio.h>
#include "ast.h"
#include "symtable.h"
#include "list.h"
#include "errors.h"
#include "scanner.h"

#ifndef IFJ_GENERATOR_H
#define IFJ_GENERATOR_H

buffer_t *code_buffer;

typedef struct {
    char *operation;
    int dst;
    int op1;
    int op2;
} code_operation_t;

typedef struct {
    char *frame; // je-li NULL, jedna se o nazev navesti, nebo typ (kdyz je numb -3)
    void *name; // je-li NULL, jedna se o &ExprTmp
    int numb; // je-li -1, nevypise sa (napr. volani funkce); je-li -2, vypise sa $epilog; je-li -3, jedna se o typ
    /* numb:
     * -1 : $<name> | <frame>@<name>    (navesti funkci nebo promenna bez cisla)
     * -2 : $<name>$epilog              (u returnu)
     * -3 : <name>                      (datovy typ, nebo konstanta bool@true)
     * -4 : double literal
     * -5 : int literal
     * -6 : string literal // je treba prevest pomoci code_convert_string(str_val)
     */
} code_operand_t;

typedef struct {
    char *name;
    code_operand_t *a;
    code_operand_t *b;
    code_operand_t *c;
} code_statement_t;

/*typedef struct {
    int op1;
    char term_type; // typ termu: 'v' 'd' 'i' 's'
    void *op2;
} code_term_move_t;*/

/**
 * Generator vysledniho kodu, jeho vstupem je koren programu a vystupem je vysledny kod zapsan na stdout
 *
 * @param program_root Ukazatel na strukturu ast_node_t
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_generator(ast_node_t *program_root);

/**
 * Generator pro kod funkci, vcetne hlavni funkce (SCOPE)
 *
 * @param function Funkce v tabulce funkci
 * @param stats_node Uzel s telem funkce
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_function(function_t *function, ast_node_t *stats_node);

/**
 * Funkce slouzici pro spracovani vsech prikazu
 *
 * @param stats Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @param list Seznam operaci
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_stats(ast_node_t *stats, char *frame, list_t *list);

/**
 * Funkce slouzici pro spracovani prikazu if
 *
 * @param if_node Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_if(ast_node_t *if_node, char *frame, list_t *list);

/**
 * Funkce slouzici pro spracovani while cyklu
 *
 * @param while_node Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_while(ast_node_t *while_node, char *frame, list_t *list);

/**
 * Funkce slouzici pro spracovani prikazu dim
 *
 * @param dim_node Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_dim(ast_node_t *dim_node, char *frame, list_t *list);

/**
 * Funkce slouzici pro spracovani prirazeni hodnoty vysledku vyrazu promennym
 *
 * @param ass_exp_node Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_ass_exp(ast_node_t *ass_exp_node, char *frame, list_t *list);

/**
 * Funkce slouzici pro spracovani prirazeni hodnoty vysledku funkci promennym

 * @param ass_fc_node Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_ass_fc(ast_node_t *ass_fc_node, char *frame, list_t *list);

/**
 * Funkce slouzici pro spracovani prikazu print
 *
 * @param print_node Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_print(ast_node_t *print_node, char *frame, list_t *list);

/**
 * Funkce slouzici pro spracovani prikazu return
 *
 * @param return_node Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_return(ast_node_t *return_node, char *frame, list_t *list);

/**
 * Funkce slouzici pro spracovani prikazu input
 *
 * @param input_node Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_input(ast_node_t *input_node, char *frame, list_t *list);

// --------------------------------------------------------------------------------

/**
 * Funkce slouzici pro spracovani podminek 
 *
 * @param cond_node Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @param label_numb Celociselna hodnota, ktera znaci cislo labelu pro spracovani if (then) nebo while (do) prikazu
 * @param label_name Nazev labelu
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_cond(ast_node_t *cond_node, char *frame, list_t *list, int label_numb, char *label_name);

/**
 * Funkce slouzici pro spracovani podminek 
 *
 * @param expr_elem Ukazatel na strukturu ast_elem_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @param var_name Ukazatel na celociselnou hodnotu, ktera znaci cislo pomocne promenne pro spracovani vyrazu v jazyku IFJcode17
 * @param data_type Adresa, kam ulozit vysledny datovy typ vyrazu
 * @param list Seznam operaci, do ktereho vkladame operace
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_expr(ast_elem_t *expr_elem, char *frame, int *var_name, char *data_type, list_t *list);


error_t code_stats_gen(char *frame, list_t *list);

/**
 * Funkce slouzici pro spracovani parametru funkci 
 *
 * @param params_node Ukazatel na strukturu ast_node_t, jejiz podstrom urcuje prubeh spracovani
 * @param frame Retezec "GF" anebo "TF" pro spravny vypis kodu IFJcode17
 * @param param_list Seznam formalnich parametru funkce
 * @return Funkce vraci enum error_t, ktery znaci chybu v prubehu programu
 */
error_t code_params(ast_node_t *params_node, char *frame, list_t *list, list_t *param_list);

/**
 * Generator cisel nazvu promennych.
 * @param frame "GF" nebo "LF"
 * @return jedinecne cislo nazvu promenne
 */
int code_varname_gen(char *frame);

/**
 * Generator cisel labelu pro konstrukce IF a WHILE.
 * @return jedinecne cislo labelu
 */
int code_label_gen();

/**
 * Prevede lokalni promennou na typ urceny formalnim parametrem a prevedenou hodnotu ulozi do docasneho ramce.
 * @param frame "LF" nebo "GF"
 * @param var Formalni parametr
 * @param real_type Typ realneho parametru
 * @param tmp_var Cislo nazvu docasne promenne
 */
void code_param_cast(char *frame, list_t *list, variable_t *var, char real_type, int tmp_var);

/**
 * Prevede lokalni (docasnou) promennou na typ urceny strukturou variable_t.
 * @param src_frame Zdrojovy ramec
 * @param dst_frame Vysledny ramec
 * @param dst_type Pozadovany typ
 * @param dst_name Nazev vysledne promenne
 * @param real_type Skutecny typ docasny promenne
 * @param tmp_var Cislo nazvu docasne promenne
 * @param list Seznam prikazu
 */
void code_cast(char *src_frame, char *dst_frame, char dst_type, char *dst_name, char real_type, int tmp_var, list_t *list);

/**
 * Zaokrouhli vstupni float a vraci float. Funkci vyuzivame pri celociselnem deleni.
 * @param frame "LF" nebo "GF"
 * @param input Cislo nazvu promenne vstupniho floatu
 * @param list Seznam operaci
 * @return Cislo nazvu promenne zaokrouhleneho floatu
 */
int code_round_float(char *frame, int input, list_t *list);

/**
 * Konvertuje retezec do formy vystupniho kodu.
 * @param src Zdrojovy retezec
 * @return Retezec bufferu
 */
char *code_convert_string(char *src);

/**
 * Prida retezec do bufferu.
 * @param string Retezec na pridani
 */
void code_buffer_append(char *string);

void code_builtin_length();

void code_builtin_substr();

void code_builtin_asc();

void code_builtin_chr();

#endif //IFJ_GENERATOR_H