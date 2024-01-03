# Multi-Tasking Codelab Question 1

## Observation

### Priorité Normale

Tout d'abord, laissons au thread qui gère les ISR, `_thread`, la priorité par défaut : "osPriorityNormal". 
Dans ce cas, la board fonctionne comme attendu, et les actions sont effectuées quasiment instantanément après l'input.
Ce cas sera celui utilisé par notre programme à la fin, et représente donc l'échantillon témoin par rapport aux autres priorité que nous allons tester.

![Alt text](priorityNormal.png)

On observe un temps de réponse 14 microsecondes.

### Priorité Basse

Maintenant mettons la priorité du thread associé au ISR à basse : 

```cpp
BikeSystem::BikeSystem()
    : _thread(osPriorityBelowNormal),
    ...
```
Lrsque l'on reset la board, voici les logs affichés.

![Alt text](priorityBelow.png)

On observe à nouveau un temps de réponse de 14 microsecondes.

### Priorité Haute

Enfin mettons la priorité du thread à haute : 

```cpp
BikeSystem::BikeSystem()
    : _thread(osPriorityAboveNormal),
    ...
```

![Alt text](priorityAbove.png)

A nouveau, on observe un temps de réponse du reset de 14 microsecondes.


## Explications 

Je trouve étrange que dans les 3 cas les temps de réponses observés soit identiques. 
J'explique cela par le fait que de base, le CPU n'est occupé qu'un 1%. Ainsi l'arrivée d'un nouvel event dans le thread pour les ISR, peut être instantanément desservi, quelquesoit la priorité dudit thread.
