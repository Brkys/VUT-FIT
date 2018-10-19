/**
 * @file scanner.c
 * @author Simeon Borko, Zdenek Brhel
 * @date 11-12 2017
 * @brief Soubor obsahujujici algoritmus lexikalne analyzy
 */

#include "scanner.h"

/* Buffer */

buffer_t *buffer_init()
{
    //inicializace bufferu
    error_t err;
    buffer_t *buffer = trash_add(sizeof(buffer_t), &err);
    if(err != E_OK)
        return NULL;
    if (buffer == NULL)
        return NULL;
    buffer->size = 0; //nastaveni implicitnich hodnot bufferu
    buffer->capacity = 0;
    buffer->string = NULL;
    return buffer;
}

bool buffer_append(buffer_t *buffer, char c)
{
    error_t err;
    if (buffer->size >= buffer->capacity) { //realokace kapacity zasobniku v pripade, ze je velikost vetsi nebo stejna jako kapacita

        if (buffer->string == NULL) { //zakladni inicializace pole
            buffer->string = trash_add(sizeof(char)*BUFF_EXTEND, &err); 
            if(err != E_OK)
                return false;
        } else { // realokace v pripade, ze jiz pole existuje
            buffer->string = trash_add_realloc(buffer->string, sizeof(char)*(BUFF_EXTEND+buffer->capacity), &err);
            if(err != E_OK)
                return false;
        }

        if (buffer->string == NULL) {
            return false; //chyba při mallocu
        }

        buffer->capacity += BUFF_EXTEND; //rozsireni kapacity
    }

    buffer->string[buffer->size++] = c; //zapsani hodnoty do zasobniku
    return true;
}


/* Token */

token_t *token_init(buffer_t *buffer, state_t state)
{
    // pridat ukoncovaci znak
    bool result = buffer_append(buffer, 0);
    if (!result)
        return NULL;

    // alokovat pamet pro token
    error_t err;
    token_t *token = trash_add(sizeof(token_t), &err);
    if(err != E_OK)
        return NULL;
    if (token == NULL)
        return NULL;

    // nastavit typ tokenu
    result = scanner_token_type(&token->type, buffer, state);
    if (!result) {
        return NULL;
    }

    // alokovat pamet pro retezec
    token->value = trash_add(sizeof(char) * buffer->size, &err);
    if(err != E_OK)
        return NULL;
    if (token->value == NULL) {
        return NULL;
    }

    // nakopirovat retezec do alokovane pameti
    strncpy(token->value, buffer->string, buffer->size);

    // resetovat buffer
    buffer->size = 0;

    // vratit ukazatel na vysledni token
    return token;
}

bool scanner_token_type(token_type_t *type, buffer_t *buffer, state_t state)
{
    switch (state) {
        case EOL:
            *type = T_EOL;
            break;

        case ID:
            // konverze na male znaky
            for (unsigned i = 0; i < buffer->size; i++)
                buffer->string[i] = (char) tolower(buffer->string[i]);

            // zda je keyword
            *type = scanner_keyword(buffer) ? T_KEYWORD : T_ID;
            break;

        case NUMB:
            *type = T_INTEGER;
            break;

        case DOUB:
        case DOUBL:
            *type = T_DOUBLE;
            break;

        case STR_DONE:
            *type = T_STRING;
            break;

        case SEMICOLON:
            *type = T_SEMICOLON;
            break;

        case BRACKET_LEFT:
            *type = T_BR_LEFT;
            break;

        case BRACKET_RIGHT:
            *type = T_BR_RIGHT;
            break;

        case COMMA:
            *type = T_COMMA;
            break;

        case OP_PLUS:
            *type = T_PLUS;
            break;

        case OP_MINUS:
            *type = T_MINUS;
            break;

        case OP_MULTIPLIER:
            *type = T_MULTIPLIER;
            break;

        case OP_SLASH:
            *type = T_SLASH;
            break;

        case OP_BACKSLASH:
            *type = T_BACKSLASH;
            break;

        case OP_EQUAL:
            *type = T_EQUAL;
            break;

        case OP_DIFF:
            *type = T_DIFF;
            break;

        case OP_LESS_THAN:
            *type = T_LESS_THAN;
            break;

        case OP_GREATER_THAN:
            *type = T_GREATER_THAN;
            break;

        case OP_LESS_EQUAL:
            *type = T_LESS_EQUAL;
            break;

        case OP_GREATER_EQUAL:
            *type = T_GREATER_EQUAL;
            break;

        default:
            return false;
    }

    return true;

}

bool scanner_keyword(buffer_t *buffer)
{   //kontrola, jestli je obsah bufferu keyword, nebo ne
    return (
            !strncmp("as", buffer->string, buffer->size) ||
            !strncmp("asc", buffer->string, buffer->size) ||
            !strncmp("declare", buffer->string, buffer->size) ||
            !strncmp("dim", buffer->string, buffer->size) ||
            !strncmp("do", buffer->string, buffer->size) ||
            !strncmp("double", buffer->string, buffer->size) ||
            !strncmp("else", buffer->string, buffer->size) ||
            !strncmp("end", buffer->string, buffer->size) ||
            !strncmp("chr", buffer->string, buffer->size) ||
            !strncmp("function", buffer->string, buffer->size) ||
            !strncmp("if", buffer->string, buffer->size) ||
            !strncmp("input", buffer->string, buffer->size) ||
            !strncmp("integer", buffer->string, buffer->size) ||
            !strncmp("length", buffer->string, buffer->size) ||
            !strncmp("loop", buffer->string, buffer->size) ||
            !strncmp("print", buffer->string, buffer->size) ||
            !strncmp("return", buffer->string, buffer->size) ||
            !strncmp("scope", buffer->string, buffer->size) ||
            !strncmp("string", buffer->string, buffer->size) ||
            !strncmp("substr", buffer->string, buffer->size) ||
            !strncmp("then", buffer->string, buffer->size) ||
            !strncmp("while", buffer->string, buffer->size) ||
            !strncmp("and", buffer->string, buffer->size) ||
            !strncmp("boolean", buffer->string, buffer->size) ||
            !strncmp("continue", buffer->string, buffer->size) ||
            !strncmp("elseif", buffer->string, buffer->size) ||
            !strncmp("exit", buffer->string, buffer->size) ||
            !strncmp("false", buffer->string, buffer->size) ||
            !strncmp("for", buffer->string, buffer->size) ||
            !strncmp("next", buffer->string, buffer->size) ||
            !strncmp("not", buffer->string, buffer->size) ||
            !strncmp("or", buffer->string, buffer->size) ||
            !strncmp("shared", buffer->string, buffer->size) ||
            !strncmp("static", buffer->string, buffer->size) ||
            !strncmp("true", buffer->string, buffer->size)
    );
}

error_t token_get(token_t **token_ptr, buffer_t *buffer)
{
    static char c = 0;
    static token_type_t last_token_type = T_ID; // jakykoliv typ tokenu krome T_EOL
    state_t state = START;
    bool internal_error = false;

    *token_ptr = NULL;

    if (c == EOF) {
        return E_OK; // ukonceni: c je true, *token_ptr je NULL
    } else if (c != 0) { // znak ziskany z minuleho spusteni
        bool result = scanner_process_char(&state, buffer, c, token_ptr, &internal_error);
        if (internal_error)
            return E_INTERNAL;
        else if (!result) {
            return E_LEXICAL;
        } else if (*token_ptr != NULL) {
            if (last_token_type == T_EOL && (*token_ptr)->type == T_EOL) {
                return token_get(token_ptr, buffer);
            }
            last_token_type = (*token_ptr)->type;
            return E_OK;
        }
    }

    int input_char;
    while ((input_char = getchar()) != EOF) {
        // over, zda je se zmesti do charu
        if (input_char > 127) {
            fprintf(stderr, "Varovani: Na vstupe je znak s ASCII hodnotou %d\n", input_char);
        }
        c = (char) input_char;

        // spracuj ziskany znak
        bool result = scanner_process_char(&state, buffer, c, token_ptr, &internal_error);

        // pokud nastala interni chyba
        if (internal_error)
            return E_INTERNAL;

        // pokud nastala lexikalni chyba
        else if (!result)
            return E_LEXICAL;

        // pokud byla lexema ulozena do tokenu
        else if (*token_ptr != NULL) {
            if (last_token_type == T_EOL && (*token_ptr)->type == T_EOL) {
                return token_get(token_ptr, buffer);
            }
            last_token_type = (*token_ptr)->type;
            return E_OK;
        }

        // jinak pokracuj
    }

    c = EOF;
    if (state != START && state != COMM_LINE) {
        bool result = scanner_process_char(&state, buffer, c, token_ptr, &internal_error);
        if (internal_error)
            return E_INTERNAL;
        else if (!result)
            return E_LEXICAL;
        else if (*token_ptr != NULL) {
            // ak bol vytvoreny novy token
            if (last_token_type == T_EOL && (*token_ptr)->type == T_EOL) {
                return token_get(token_ptr, buffer);
            }
            last_token_type = (*token_ptr)->type;
            return E_OK;
        } else {
            // ak nebol vytvoreny novy token
            return E_LEXICAL;
        }
    }

    return E_OK;

}


/* Zpracovavani znaku */

bool scanner_process_char(state_t *state, buffer_t *buffer, char ch, token_t **token_ptr, bool *internal_error)
{
    // pridat znak do bufferu, pokud na zaklade stavu muze byt pridan
    bool result = scanner_add(state, buffer, ch, internal_error);

    // pokud nastala interni chyba
    if (*internal_error)
        return false;

    // pokud byl znak pridan do bufferu
    else if (result)
        return true;

    // plati, ze znak nebyl pridan do bufferu

    bool final = scanner_is_final_state(*state);

    // automat skoncil v nefinalnim stavu
    if (!final)
        return false;

    // plati, ze automat skoncil ve finalnim stavu

    // vytvorit token
    *token_ptr = token_init(buffer, *state);
    if (!*token_ptr) {
        *internal_error = true;
        return false;
    }

    return true;
}

bool scanner_add(state_t *state, buffer_t *buffer, char c, bool *internal_error)
{
    switch (*state) {
        case START:
            if(c == '\n')
                *state = EOL;
            else if(scanner_can_start_id(c))
                *state = ID;
            else if(isdigit(c))
                *state = NUMB;
            else if(c == '!')
                *state = EXCLAM;
            else if(c == '\'')
            {
                *state = COMM_LINE;
                return true;
            }
            else if(c == ';')
                *state = SEMICOLON;
            else if(c == '(')
                *state = BRACKET_LEFT;
            else if(c == ')')
                *state = BRACKET_RIGHT;
            else if(c == ',')
                *state = COMMA;
            else if(c == '+')
                *state = OP_PLUS;
            else if(c == '-')
                *state = OP_MINUS;
            else if(c == '*')
                *state = OP_MULTIPLIER;
            else if(c == '/')
                *state = OP_SLASH;
            else if(c == '\\')
                *state = OP_BACKSLASH;
            else if(c == '=')
                *state = OP_EQUAL;
            else if(c == '<')
                *state = OP_LESS_THAN;
            else if(c == '>')
                *state = OP_GREATER_THAN;
            else if(isspace(c))
                return true;
            else
                return false;
            break;
//---------------------------------------------------------------------
        case ID:
            if(!scanner_can_be_in_id(c))
                return false;
            break;
//---------------------------------------------------------------------
        case NUMB:
            if(c == '.')
                *state = NUMB_DOT;
            else if(c == 'e' || c == 'E')
                *state = DOUB_E;
            else if(!isdigit(c))
                return false;
            break;
//---------------------------------------------------------------------
        case NUMB_DOT:
            if(isdigit(c))
                *state = DOUB;
            else
                return false;
            break;
//---------------------------------------------------------------------
        case DOUB:
            if(c == 'E' || c == 'e')
                *state = DOUB_E;
            else if (!isdigit(c))
                return false;
            break;
//---------------------------------------------------------------------
        case DOUB_E:
            if(c == '+' || c == '-')
                *state = DOUB_ES;
            else if(isdigit(c))
                *state = DOUBL;
            else
                return false;
            break;
//---------------------------------------------------------------------
        case DOUB_ES:
            if(isdigit(c))
                *state = DOUBL;
            else
                return false;
            break;
//---------------------------------------------------------------------
        case DOUBL:
            if(!isdigit(c))
                return false;
            break;
//---------------------------------------------------------------------
        case EXCLAM:
            if(c == '"')
                *state = STR_INSIDE;
            else
                return false;
            break;
//---------------------------------------------------------------------
        case STR_INSIDE:
            if (c == '"')
                *state = STR_DONE;
            else if (c == '\\')
                *state = STR_ESCAPE;
            else if (c <= 31)
                return false;
            break;
//---------------------------------------------------------------------
        case STR_ESCAPE:
            if (c == '"' || c == 'n' || c == 't' || c == '\\')
                *state = STR_INSIDE;
            else if (isdigit(c) && c >= '0' && c <= '2')
                *state = STR_ESCAPE_DEC_1;
            else
                return false; // urobene tak, ze samotny backslash nemuze byt
            break;
//---------------------------------------------------------------------
        case STR_ESCAPE_DEC_1:
            if (isdigit(c) && (buffer->string[buffer->size - 1] < '2' || c <= '5'))
                *state = STR_ESCAPE_DEC_2;
            else
                return false; // za backslashom musi byt korektni escape sekvence
            break;
//---------------------------------------------------------------------
        case STR_ESCAPE_DEC_2: {

            char c1 = buffer->string[buffer->size - 2];
            char c2 = buffer->string[buffer->size - 1];

            if (isdigit(c) &&
                    (
                        (c1 == '0' && (c2 > '0' || c > '0'))
                        || (c1 == '1')
                        || (c1 == '2' && (c2 < '5' || c <= '5'))
                    )) {
                *state = STR_INSIDE;
            }
            else
                return false;
            break;
        }
//---------------------------------------------------------------------
        case STR_DONE:
            return false;
//---------------------------------------------------------------------
        case COMM_LINE:
            if(c != '\n')
                return true;
            else
                *state = EOL;
            break;
//---------------------------------------------------------------------
        case COMM_BLOCK_INSIDE:
            if (c == '\'')
                *state = COMM_BLOCK_APOST;
            return true;
            break;
//---------------------------------------------------------------------
        case COMM_BLOCK_APOST:
            if (c == '/')
                *state = START;
            else
                *state = COMM_BLOCK_INSIDE;
            return true;
            break;
//---------------------------------------------------------------------
        case EOL:
            if (c == '\n')
                return true;
            return false;
//---------------------------------------------------------------------
        case SEMICOLON:
        case BRACKET_LEFT:
        case BRACKET_RIGHT:
        case COMMA:
        case OP_PLUS:
        case OP_MINUS:
        case OP_MULTIPLIER:
        case OP_BACKSLASH:
        case OP_EQUAL:
        case OP_DIFF:
        case OP_LESS_EQUAL:
        case OP_GREATER_EQUAL:
            return false;
//---------------------------------------------------------------------
        case OP_SLASH:
            if(c == '\'') {
                buffer->size = 0;
                *state = COMM_BLOCK_INSIDE;
                return true;
            }
            else
                return false;
            break;
//---------------------------------------------------------------------
        case OP_LESS_THAN:
            if(c == '>')
                *state = OP_DIFF;
            else if(c == '=')
                *state = OP_LESS_EQUAL;
            else
                return false;
            break;
//---------------------------------------------------------------------
        case OP_GREATER_THAN:
            if(c == '=')
                *state = OP_GREATER_EQUAL;
            else
                return false;
            break;
//---------------------------------------------------------------------
        default:
            *internal_error = true;
            return false;
    }

    bool result = buffer_append(buffer, c);
    if (!result) {
        *internal_error = true;
        return false;
    }

    return true;
}

bool scanner_can_start_id(char c)
{
//ID muze obsahovat: male pismena ||podtrznik || velke pismena
    return ((c >= 97 && c <= 122) || c == '_' || (c >= 65 && c <= 90));
}

bool scanner_can_be_in_id(char c)
{
    //kontroluje, zda-li znak je cislo, pripadne je li mozne s timto znakem zacit ID
    return (isdigit(c) || scanner_can_start_id(c));
}

bool scanner_is_final_state(state_t state)
{
    switch (state) {
        case EOL:
        case ID:
        case NUMB:
        case DOUB:
        case DOUBL:
        case STR_DONE:
        case SEMICOLON:
        case BRACKET_LEFT:
        case BRACKET_RIGHT:
        case COMMA:
        case OP_PLUS:
        case OP_MINUS:
        case OP_MULTIPLIER:
        case OP_SLASH:
        case OP_BACKSLASH:
        case OP_EQUAL:
        case OP_DIFF:
        case OP_LESS_THAN:
        case OP_GREATER_THAN:
        case OP_LESS_EQUAL:
        case OP_GREATER_EQUAL:
            return true;

        default:
            return false;
    }
}

bool token_to_int(token_t *token, int **dst) {
    error_t err;
    *dst = (int*) trash_add(sizeof(int), &err);
    if(err != E_OK)
        return false;
    if (!*dst)
        return false;
    **dst = (int) strtol(token->value, NULL, 10); // TODO mame kontrolovat rozsah int
    return true;
}

bool token_to_double(token_t *token, double **dst) {
    error_t err;
    *dst = (double*) trash_add(sizeof(double), &err);
    if (!*dst)
        return false;
    **dst = strtod(token->value, NULL);
    return true;
}
bool token_to_string(token_t *token, char **dst) {
    size_t length = strlen(token->value);
    error_t err;
    *dst = (char *) trash_add((length-2) * sizeof(char), &err);
    if (!*dst)
        return false;
    for (size_t i = 0; i < length-3; i++) {
        (*dst)[i] = token->value[i+2];
    }
    (*dst)[length-3] = 0;
    return true;
}

/* Konec souboru scanner.c */
