/**
 * @file precedence.h
 * @author Filip Borcik, Simeon Borko
 * @date 11-12 2017
 * @brief Soubor obsahujujici definice vyctovych typu, struktur a hlavicky funkci pro precedencni syntaktickou analyzu
 */

#include "stack.h"
#include "scanner.h"
#include "ast.h"
#include "symtable.h"
#include "errors.h"
#include "garbage.h"


#ifndef IFJ_PRECEDENCE_H
#define IFJ_PRECEDENCE_H

typedef enum {
	PT_MULTIPLIER,
	PT_SLASH,
	PT_BACKSLASH,
	PT_PLUS,
	PT_MINUS,

	PT_EQUAL,
	PT_DIFF,
	PT_LESS_THAN,
	PT_LESS_EQUAL,
	PT_GREATER_THAN,
	PT_GREATER_EQUAL,

	PT_BRACKET_LEFT,
	PT_BRACKET_RIGHT,

	PT_ID,
	PT_EMPTY
} precedence_token_t;

typedef enum {
	IN_TABLE_PT,
	IN_HEADER_PT,
	NONTERMINAL_PT
} precedence_op_type_t;

typedef struct {
  precedence_token_t type;
  token_t *token; 			// v pripade, ze type je 'ID'
} precedence_header_t;

typedef struct {
  char data_type;			// 's' string, 'd' double, 'i' integer, 'b' boolean (pro podminkove vyrazy)
  ast_elem_t *ast;
} precedence_nonterminal_t;



/**
* Najde nejvyssie polozeny znak zahlavi tabulky
*
* @param stack Ukazatel na strukturu stack_t
* @return V pripade nalezeni vraci ukazatel na prvek zasobniku, v opacnem pripade funkce vraci NULL
*/
stack_el_ptr header_top(stack_t *stack);

/**
* Implementovany algoritmus precedencni syntakticke analyzy
*
* @param token_ptr Ukazatel na ukazatel ukazujici na strukturu token_t, vstupny token
* @param buffer Ukazatel na strukturu buffer_t, vstupny buffer
* @param ht Ukazatel na strukturu hashtable_t, hashovaci tabulka
* @param condition Zda volajici ocekava podminkovy vyraz
* @param elem_ptr Ukazatel na ukazatel ukazujici na strukturu ast_elem_t, vystup precedencni analyzy
* @param final_data_type Vysledny datovy typ vyrazu
* @return Funkce vraci vyctovy typ error_t, podle toho jestli nastala nebo nenastala chyba
*/
error_t precedence_algorithm(token_t **token_ptr, buffer_t *buffer,
							 hashtable_t *ht, bool condition,
							 ast_elem_t **elem_ptr, char *final_data_type);

/**
* Prevede typ tokenu na typ pozadovan v precedencni SA. Pokud vstupni token neni vhodny pro precedencni analyzu,
* povazuje se to za konec vstupu.
*
* @param token Vstupni token (ze scanneru)
* @return Typ precedencniho tokenu nebo PT_EMPTY kdyz neni vstupny token vhodny
*/
precedence_token_t precedence_token_type (token_t *token);

/**
* Kontruluje jestli je splneno nektere z pravidel precedence
*
* @param stack Ukazatel na strukturu stack_t
* @param ht Ukazatel na strukturu hashtable_t, hashovaci tabulka
* @return V pripade spravnosti funkce vraci 'true', v opacnem pripade funkce vraci 'false'
*/
error_t precedence_rule_control (stack_t *stack, hashtable_t *ht);

/**
* Vytvori neterminal A podle pravidla zameny <y za A, pricemz y == ID
*
* @param token Ukazatel na strukturu token_t
 * @param ht Ukazatel na strukturu hashtable_t, hashovaci tabulka promennych
 * @param err Ukazatel na vyctovy typ error_t, indikuje chybu v prubehu funkce
* @return Vraci vytvoreny neterminal, v pripade chyby funkce vraci 'NULL'
*/
precedence_nonterminal_t *precedence_id_nonterm(token_t *token, hashtable_t *ht, error_t *err);

/**
* Vytvori neterminal A podle pravidla zameny <y za A, pricemz y == E op E  a op je nektery z operatoru
*
* @param e1 Ukazatel na strukturu neterminalu precedence_nonterminal_t
* @param e2 Ukazatel na strukturu neterminalu precedence_nonterminal_t
* @param op Vyctovy typ precedence_token_t, ktery urcuje typ operatoru
* @return Vraci vytvoreny neterminal, v pripade chyby funkce vraci 'NULL'
*/
precedence_nonterminal_t *precedence_op_nonterm(precedence_nonterminal_t *e1, precedence_nonterminal_t *e2, precedence_token_t op, error_t *err);

#endif // IFJ_PRECEDENCE_H
