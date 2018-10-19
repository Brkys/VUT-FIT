/**
 * @file main.c
 * @author Simeon Borko
 * @date 11-12 2017
 * @brief Soubor obsahujujici hlavni funkci programu
 */

#include "scanner.h"
#include "garbage.h"
#include "recursive_descent.h"
#include "generator.h"
#include "ast.h"

int main(int argc, char *argv[])
{
    if(trash_init(&trash) == E_INTERNAL)
            return E_INTERNAL;
    if (argc >= 2)
        freopen(argv[1], "r", stdin);

    token_t *token = NULL;
    buffer_t *buffer = buffer_init();
    if (!buffer) {
        return E_INTERNAL;
    }

    ast_elem_t *root_el = NULL;
    error_t err = recursive_descent(buffer, &token, &root_el);
    if (err != E_OK) {
        trash_empty();
        fprintf(stderr, "Nastala chyba s cislem %d\n", err);
        return err;
    }

    err = code_generator(root_el->data);

    trash_empty();
    fprintf(stderr, "Navratovy kod: %d\n", err);
    return err;

    /*freopen("data/s_in_1", "r", stdin);

    error_t result;
    bool internal_error = false;




    do {
        result = token_get(&token, buffer);
        printf("Result: %d\n", result);
        if (internal_error) {
            printf("V cykle, exit 99\n");
            if (token)
                token_destroy(token);
            buffer_destroy(buffer);
            exit(99);
        }
        if (token != NULL) {
            printf("Token ID:  %d\nToken VAL: [%s]\n\n", token->type, (char *) token->value);
            token_destroy(token);
        }
    } while (token != NULL);

    buffer_destroy(buffer);

    return result;*/
}
