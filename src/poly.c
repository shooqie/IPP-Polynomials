#include <stdlib.h>
#include <stdbool.h>
#include "poly.h"

/**
 * Zwraca większą z dwóch liczb
 * @param a
 * @param b
 * @return max(@p a, @p b)
 */
static inline poly_exp_t max(poly_exp_t a, poly_exp_t b) {
    return a > b ? a : b;
}

/**
 * Zwraca x^exp
 * @param x : podstawa
 * @param exp : wykładnik
 * @return x^exp
 */
poly_coeff_t ipow(poly_coeff_t x, poly_exp_t exp) {
    poly_coeff_t res = 1;
    while (exp > 0) {
        if (exp & 1) {
            res *= x;
        }
        exp >>= 1;
        x *= x;
    }
    return res;
}

/**
 * Tworzy kopię jednomianu i zwraca wskaźnik na nią.
 * Zwracany wskaźnik jest tworzony za pomocą funkcji @p malloc() i może
 * być bezpiecznie usunięty z pamięci przy użyciu @p free().
 * @param m : Jednomian
 * @return Wskaźnik na kopię jednomianu
 */
Mono *MonoMemCopy(const Mono m) {
    Mono *res = malloc(sizeof(Mono));
    res->exp = m.exp;
    res->p = PolyClone(&m.p);
    res->next = NULL;
    return res;
}

/**
 * Przekształca wielomian @p p do postaci normalnej (modyfikując go)
 * przy pewnych założeniach:
 * - Jeżeli @p p jest wielomianem stałym, to jest on wielomianem zerowym
 * - Wszystkie wielomiany zmiennej >= 1 wchodzące w skład @p p
 *   są w postaci normalnej
 * @param p : Wskaźnik na wielomian
 */
void PolyNormalize(Poly *p) {
    if (PolyIsCoeff(p)) {
        return;
    }
    // jeżeli wielomian jest postaci c * x^0
    if (p->head->exp == 0 && p->head->next == NULL &&
        PolyIsCoeff(&p->head->p)) {
        poly_coeff_t c = (p->head->p).coeff;
        PolyDestroy(p);
        *p = PolyFromCoeff(c);
        return;
    }
    while (p->head != NULL && PolyIsZero(&p->head->p)) {
        Mono *temp = p->head;
        p->head = p->head->next;
        free(temp);
    }
    if (p->head == NULL) {
        return;
    }
    Mono *p_head = p->head;
    while (p_head->next != NULL) {
        if (PolyIsZero(&p_head->next->p)) {
            Mono *temp = p_head->next;
            p_head->next = temp->next;
            free(temp);
        }
        else {
            p_head = p_head->next;
        }
    }
    // jeżeli wielomian nadal jest postaci c * x^0
    if (p->head->exp == 0 && p->head->next == NULL &&
        PolyIsCoeff(&p->head->p)) {
        poly_coeff_t c = p->head->p.coeff;
        PolyDestroy(p);
        *p = PolyFromCoeff(c);
    }
}

/**
 * Tworzy kopię wielomianu pomnożoną przez stałą
 * @param p : wskaźnik na wielomian
 * @param c : stała
 * @return c * p
 */
Poly PolyCloneTimesC(const Poly *p, poly_coeff_t c) {
    if (c == 0) {
        return PolyZero();
    }
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff * c);
    }
    Mono *res_head = malloc(sizeof(Mono));
    Mono *res_last = res_head;
    Mono *p_head = p->head;

    res_last->p = PolyCloneTimesC(&p_head->p, c);
    res_last->exp = p_head->exp;
    p_head = p_head->next;

    Mono *m;
    while (p_head != NULL) {
        m = malloc(sizeof(Mono));
        m->p = PolyCloneTimesC(&p_head->p, c);
        m->exp = p_head->exp;
        res_last->next = m;
        res_last = res_last->next;
        p_head = p_head->next;
    }
    res_last->next = NULL;
    return (Poly) {.head = res_head, .coeff = 0};
}

void PolyMulByConstant(Poly *p, poly_coeff_t c) {
    if (PolyIsCoeff(p)) {
        p->coeff *= c;
    }
    Mono *head = p->head;
    while (head != NULL) {
        PolyMulByConstant(&head->p, c);
        head = head->next;
    }
    PolyNormalize(p);
}

void AppendPoly(Poly *p, Poly *q, poly_exp_t e) {
    if (p->head == NULL) {
        Mono *new_head = malloc(sizeof(Mono));
        new_head->p = *q;
        new_head->exp = e;
        new_head->next = NULL;
        p->head = new_head;
        return;
    }
    if (p->head->exp > e) {
        Mono *new_head = malloc(sizeof(Mono));
        new_head->p = *q;
        new_head->exp = e;
        new_head->next = p->head;
        p->head = new_head;
        return;
    }
    Mono *p_head = p->head;
    for (;;) {
        if (p_head->exp == e) {
            Poly p_temp = p_head->p;
            p_head->p = PolyAdd(&p_head->p, q);
            PolyDestroy(&p_temp);
            PolyDestroy(q);
            break;
        }
        else if (p_head->next == NULL) {
            Mono *new_last = malloc(sizeof(Mono));
            new_last->p = *q;
            new_last->exp = e;
            new_last->next = NULL;
            p_head->next = new_last;
            break;
        }
        else if (p_head->exp < e && p_head->next->exp > e) {
            Mono *p_head_next = p_head->next;
            Mono *middle = malloc(sizeof(Mono));
            middle->p = *q;
            middle->exp = e;
            middle->next = p_head_next;
            p_head->next = middle;
            break;
        }
        p_head = p_head->next;
    }
}

void PolyDestroy(Poly *p) {
    Mono *p_head = p->head;
    while (p_head != NULL) {
        Mono *temp = p_head->next;
        PolyDestroy(&p_head->p);
        free(p_head);
        p_head = temp;
    }
}

Poly PolyClone(const Poly *p) {
    return PolyCloneTimesC(p, 1);
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    bool pIsCoeff = PolyIsCoeff(p);
    bool qIsCoeff = PolyIsCoeff(q);
    if (pIsCoeff && qIsCoeff) {
        return PolyFromCoeff(p->coeff + q->coeff);
    }
    else if (pIsCoeff) {
        Mono *res_head = malloc(sizeof(Mono));
        Mono *res_last = res_head;
        Mono *q_head = q->head;
        res_last->exp = 0;
        if (q_head->exp == 0) {
            res_last->p = PolyAdd(p, &(q_head->p));
            q_head = q_head->next;
        }
        else {
            res_last->p = PolyFromCoeff(p->coeff);
        }
        while (q_head != NULL) {
            Mono *m = MonoMemCopy(*q_head);
            res_last->next = m;
            res_last = res_last->next;
            q_head = q_head->next;
        }
        res_last->next = NULL;
        Poly res = (Poly) {.head = res_head, .coeff = 0};
        PolyNormalize(&res);
        return res;
    }
    else if (qIsCoeff) {
        return PolyAdd(q, p);
    }
    else {
        Mono *res_head = malloc(sizeof(Mono));
        Mono *res_last = res_head;
        Mono *p_head = p->head;
        Mono *q_head = q->head;

        if (p_head->exp == q_head->exp) {
            res_last->exp = p_head->exp;
            res_last->p = PolyAdd(&p_head->p, &q_head->p);
            p_head = p_head->next;
            q_head = q_head->next;
        }
        else if (p_head->exp < q_head->exp) {
            res_last->p = PolyClone(&p_head->p);
            res_last->exp = p_head->exp;
            p_head = p_head->next;
        }
        else {
            res_last->p = PolyClone(&q_head->p);
            res_last->exp = q_head->exp;
            q_head = q_head->next;
        }

        while (p_head != NULL && q_head != NULL) {
            Mono *m = malloc(sizeof(Mono));
            if (p_head->exp == q_head->exp) {
                m->exp = p_head->exp;
                m->p = PolyAdd(&(p_head->p), &(q_head->p));
                p_head = p_head->next;
                q_head = q_head->next;
            }
            else if (p_head->exp < q_head->exp) {
                m->exp = p_head->exp;
                m->p = PolyClone(&p_head->p);
                p_head = p_head->next;
            }
            else {
                m->exp = q_head->exp;
                m->p = PolyClone(&q_head->p);
                q_head = q_head->next;
            }
            res_last->next = m;
            res_last = res_last->next;
        }

        while (p_head != NULL) {
            res_last->next = MonoMemCopy(*p_head);
            res_last = res_last->next;
            p_head = p_head->next;
        }
        while (q_head != NULL) {
            res_last->next = MonoMemCopy(*q_head);
            res_last = res_last->next;
            q_head = q_head->next;
        }

        res_last->next = NULL;
        Poly res = (Poly) {.coeff = 0, .head = res_head};
        PolyNormalize(&res);
        return res;
    }
}

Poly PolyNeg(const Poly *p) {
    return PolyCloneTimesC(p, -1);
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly minus_q = PolyNeg(q);
    Poly res = PolyAdd(p, &minus_q);
    PolyDestroy(&minus_q);
    return res;
}

/**
 * Funkcja porównująca dwa jednomiany według ich wykładników
 * @param m1 : wskaźnik na pierwszy jednomian
 * @param m2 : wskaźnik na drugi jednomian
 * @return -1 jeżeli wykładnik pierwszego jednomianu jest mniejszy
 * od wykładnika drugiego jednomianu, 1 jeśli jest większy,
 * 0 jeśli wykładniki są równe
 */
int MonoCmp(const void *m1, const void *m2) {
    poly_exp_t m1_exp = ((Mono *) m1)->exp;
    poly_exp_t m2_exp = ((Mono *) m2)->exp;
    return m1_exp < m2_exp ? -1 : m1_exp > m2_exp;
}

Poly PolyAddMonos(unsigned count, const Mono monos[]) {
    if (count == 0) {
        return PolyZero();
    }

    Mono temp_arr[count];
    for (unsigned int i = 0; i < count; i++) {
        temp_arr[i] = monos[i];
    }

    qsort((void *) temp_arr, count, sizeof(Mono), MonoCmp);

    Mono *res_head = malloc(sizeof(Mono));
    *res_head = temp_arr[0];
    Mono *res_last = res_head;

    for (unsigned int i = 1; i < count; i++) {
        if (res_last->exp == temp_arr[i].exp) {
            Poly temp = res_last->p;
            Poly current = temp_arr[i].p;
            res_last->p = PolyAdd(&temp, &current);
            PolyDestroy(&temp);
            PolyDestroy(&current);
        } else {
            Mono *m = malloc(sizeof(Mono));
            *m = temp_arr[i];
            res_last->next = m;
            res_last = res_last->next;
        }
    }

    res_last->next = NULL;
    Poly res = (Poly) {.head = res_head, .coeff = 0};
    PolyNormalize(&res);
    return res;
}

int PolyLen(const Poly *p) {
    int count = 0;
    for (Mono *p_head = p->head; p_head != NULL; p_head = p_head->next) {
        count++;
    }
    return count;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsZero(p) || PolyIsZero(q)) {
        return PolyZero();
    }
    bool pIsCoeff = PolyIsCoeff(p);
    bool qIsCoeff = PolyIsCoeff(q);
    if (pIsCoeff && qIsCoeff) {
        return PolyFromCoeff(p->coeff * q->coeff);
    }
    if (pIsCoeff) {
        return PolyCloneTimesC(q, p->coeff);
    }
    if (qIsCoeff) {
        return PolyCloneTimesC(p, q->coeff);
    }
    Mono *arr = malloc(PolyLen(p) * PolyLen(q) * sizeof(Mono));
    unsigned int count = 0;
    for (Mono *p_head = p->head; p_head != NULL; p_head = p_head->next) {
        for (Mono *q_head = q->head; q_head != NULL; q_head = q_head->next) {
            arr[count] = (Mono) {
                    .p = PolyMul(&p_head->p, &q_head->p),
                    .exp = p_head->exp + q_head->exp,
                    .next = NULL
            };
            count++;
        }
    }
    Poly res = PolyAddMonos(count, arr);
    free(arr);
    return res;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p)) {
        return PolyIsCoeff(q) && p->coeff == q->coeff;
    }
    Mono *p_head = p->head;
    Mono *q_head = q->head;
    while (p_head != NULL && q_head != NULL) {
        if (p_head->exp != q_head->exp || !PolyIsEq(&p_head->p, &q_head->p)) {
            return false;
        }
        p_head = p_head->next;
        q_head = q_head->next;
    }
    return p_head == NULL && q_head == NULL;
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p)) {
        return -1;
    }
    if (PolyIsCoeff(p)) {
        return 0;
    }
    Mono *p_head = p->head;
    poly_exp_t res = -1;
    while (p_head != NULL) {
        res = max(res, p_head->exp + PolyDeg(&p_head->p));
        p_head = p_head->next;
    }
    return res;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx) {
    if (PolyIsZero(p)) {
        return -1;
    }
    if (PolyIsCoeff(p)) {
        return 0;
    }
    if (var_idx == 0) {
        Mono *p_head = p->head;
        while (p_head->next != NULL) {
            p_head = p_head->next;
        }
        return p_head->exp;
    }

    poly_exp_t res = -1;
    Mono *p_head = p->head;
    while (p_head != NULL) {
        res = max(res, PolyDegBy(&p_head->p, var_idx - 1));
        p_head = p_head->next;
    }

    return res;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff);
    }
    Poly res = PolyZero();
    for (Mono *p_head = p->head; p_head != NULL; p_head = p_head->next) {
        poly_coeff_t e = ipow(x, p_head->exp);
        Poly t = p_head->p;
        if (PolyIsCoeff(&t)) {
            Poly w = PolyFromCoeff(t.coeff * e);
            AppendPoly(&res, &w, 0);
        } else {
            for (Mono *t_head = t.head; t_head != NULL; t_head = t_head->next) {
                poly_coeff_t new_e = t_head->exp;
                Poly w = PolyCloneTimesC(&t_head->p, e);
                AppendPoly(&res, &w, new_e);
            }
        }
    }
    PolyNormalize(&res);
    return res;
}
