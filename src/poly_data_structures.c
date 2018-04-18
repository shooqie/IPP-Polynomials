/** @file
   Implementacja struktur danych do obsługi wielomianów wielu zmiennych

   @author Paweł Brzeziński <pb385254@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-25
*/

#include <assert.h>
#include "poly_data_structures.h"

Poly PolyStackPop(PolyStack *s) {
    assert(s->size > 0);
    PolyStackNode *top = s->top;
    Poly ret = top->value;
    s->top = top->next;
    s->size -= 1;
    free(top);
    return ret;
}

void PolyStackPush(Poly value, PolyStack *s) {
    PolyStackNode *new_top = malloc(sizeof(PolyStackNode));
    new_top->value = value;
    new_top->next = s->top;
    s->top = new_top;
    s->size += 1;
}

void PolyStackDestroy(PolyStack *s) {
    PolyStackNode *top = s->top;
    PolyStackNode *temp;
    while (top != NULL) {
        temp = top->next;
        PolyDestroy(&top->value);
        free(top);
        top = temp;
    }
}

void ArrayAdd(Mono n, MonoArray *array) {
    if (ArrayIsEmpty(array)) {
        array->cache = malloc(sizeof(Mono) * INITIAL_ARRAY_SIZE);
    }
    else if (array->cur_index >= array->size) {
        array->cache = realloc(
                array->cache,
                ARRAY_SIZE_MUL_FACTOR * array->size * sizeof(Mono)
        );
    }
    array->cache[array->cur_index++] = n;
    array->size++;
}

















