/** @file
   Interfejs struktur danych do obsługi wielomianów wielu zmiennych

   @author Paweł Brzeziński <pb385254@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-25
*/

#pragma once

#include <stdlib.h>
#include <assert.h>
#include "poly.h"

/**
 * Początkowy rozmiar pustej tablicy
 */
#define INITIAL_ARRAY_SIZE 50

/**
 * Mnożnik - przy przekroczeniu rozmiaru tablicy, jej nowy rozmiar to
 * @p ARRAY_SIZE_MUL_FACTOR * [poprzedni rozmiar tablicy]
 */
#define ARRAY_SIZE_MUL_FACTOR 2

/**
 * Struktura pojedynczego node'a stosu
 */
typedef struct PolyStackNode {
    Poly value; ///< wielomian
    struct PolyStackNode *next; ///< wskaźnik na następny node
} PolyStackNode;

/**
 * Struktura stosu przechowującego wielomiany
 */
typedef struct PolyStack {
    PolyStackNode *top; ///< wierzchołek stosu
    unsigned int size; ///< rozmiar stosu
} PolyStack;

/**
 * Zwraca pusty stos
 * @return pusty stos
 */
static inline PolyStack EmptyStack() {
    return (PolyStack) {.top = NULL, .size = 0};
}

/**
 * Zwraca true jeśli stos jest pusty, false w przeciwnym wypadku
 * @param[in] stack : wskaźnik na stos
 * @return czy stos jest pusty?
 */
static inline bool PolyStackIsEmpty(PolyStack *stack) {
    return stack->top == NULL;
}

/**
 * Usuwa wielomian ze stosu i zwraca go (zakłada że stos jest niepusty)
 * @param[in] stack : wskaźnik na stos
 * @return wielomian ze stosu
 */
Poly PolyStackPop(PolyStack *stack);

/**
 * Wstawia wielomian na wierzchołek stosu
 * @param[in] poly : wielomian
 * @param[in] stack : wskaźnik na stos
 */
void PolyStackPush(Poly poly, PolyStack *stack);

/**
 * Niszczy stos i wszystkie wielomiany w nim się znajdujące
 * @param[in] stack : wskaxnik na stos
 */
void PolyStackDestroy(PolyStack *stack);

/**
 * Zwraca wielomian z wierzchołka stosu nie usuwając go
 * @param[in] stack : wskaźnik na stos
 * @return wielomian z wierzchołka stosu
 */
static inline Poly PolyStackPeek(PolyStack *stack) {
    assert(stack->top != NULL);
    return stack->top->value;
}

/**
 * Struktura dynamicznie alokowanej tablicy jednomianów
 */
typedef struct MonoArray {
    Mono *cache; ///< tablica w której przechowywane są jednomiany
    unsigned int cur_index; ///< indeks pod którym znajdzie się jednomian pod jego dodaniu do tablicy
    unsigned int size; ///< rozmiar tablicy
} MonoArray;

/**
 * Zwraca pustą tablicę
 * @return pusta tablica
 */
static inline MonoArray emptyArray() {
    return (MonoArray) {.cache = NULL, .size = 0, .cur_index = 0};
}

/**
 * Zwraca true jeśli tablica jest pusta, false w przeciwnym wypadku
 * @param[in] array : wskaźnik na tablicę
 * @return czy tablica jest pusta?
 */
static inline bool ArrayIsEmpty(MonoArray *array) {
    return array->cache == NULL;
}

/**
 * Dodaje jednomian do tablicy
 * @param[in] mono : jednomian 
 * @param[in] array : wskaźnik na tablicę
 */
void ArrayAdd(Mono mono, MonoArray *array);

/**
 * Usuwa tablicę z pamięci (!bez usuwania jej zawartości!)
 * @param[in] array : wskaźnik na tablicę
 */
static inline void ArrayDestroy(MonoArray *array) {
    free(array->cache);
}
