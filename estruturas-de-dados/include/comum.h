#ifndef COMUM_H
#define COMUM_H

/*
 * Tipos de função usados por todas as estruturas.
 *
 * As estruturas guardam ponteiros genéricos (void *), então quem usa
 * diz como comparar e como liberar cada elemento.
 */

/* Libera um elemento guardado na estrutura (pode ser NULL = não liberar) */
typedef void (*FuncaoLiberar)(void *dado);

/* Compara dois elementos: < 0 se a < b, 0 se iguais, > 0 se a > b */
typedef int (*FuncaoComparar)(const void *a, const void *b);

/* Função chamada para cada elemento ao percorrer a estrutura */
typedef void (*FuncaoVisitar)(void *dado, void *contexto);

#endif /* COMUM_H */
