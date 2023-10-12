# Exercises related to Improving Software Quality

## Exercise 1

Soit le code suivant :

```cpp
int division(int x) {
    return 10/x;
}

int main () {
    ...
    int div = division(2);
    int div2 = division(0);
    ...
}
```

L'execution de cppcheck nous donne le résultat suivant. Il nous avertit que nous essayons de faire une division par 0.

```
cppcheck: Windows 64-bit binaries currently default to the 'win64' platform. Starting with Cppcheck 2.13 they will default to 'native' instead. Please
specify '--platform=win64' explicitly if you rely on this.
Checking main.cpp ...
main.cpp:29:14: error: Division by zero. [zerodiv]
    return 10/x;
             ^
main.cpp:46:25: note: Calling function 'division', 1st argument '0' value is 0
    int div2 = division(0);
                        ^
main.cpp:29:14: note: Division by zero
    return 10/x;
             ^
Checking main.cpp: LED1;MBED_TEST_MODE=0...
Checking main.cpp: MBED_CONF_MBED_TRACE_ENABLE...
Checking main.cpp: MBED_CONF_MBED_TRACE_ENABLE;MBED_TEST_MODE=0...
Checking main.cpp: MBED_TEST_MODE=0...
Active checkers: 60/565
```

Corrigons ce problème en modifiant la fonction :

```cpp
int division(int x) {
    if (x == 0) return 0;
    return 10 / x;
}
```

Et maintenant re-éxécutons cppcheck :

```
cppcheck: Windows 64-bit binaries currently default to the 'win64' platform. Starting with Cppcheck 2.13 they will default to 'native' instead. Please specify '--platform=win64' explicitly if you rely on this.
Checking main.cpp ...
Checking main.cpp: LED1;MBED_TEST_MODE=0...
Checking main.cpp: MBED_CONF_MBED_TRACE_ENABLE...
Checking main.cpp: MBED_CONF_MBED_TRACE_ENABLE;MBED_TEST_MODE=0...
Checking main.cpp: MBED_TEST_MODE=0...
Active checkers: 60/565
```

Cette fois-ci aucune erreur ! Le problème a bien été résolu.

## Exercise 2

1. `^main.cpp` signifie que pre-commit doit s'exécuter sur les fichiers commencant par 'main cpp'. Il s'agit d'une regex, '^' marque le début de la string. Attention, en regex '.' matche n'importe quel charactère.
2. Pour run les hooks sur un fichier 'main.cpp' et sur le contenu d'un dossier 'my_folder', il faudrait mettre : `files: (^main.cpp|my_folder\/) `
3. `[--std=c++14]` signifie que nous utilisons la version 14 cpp.
4. `language: system` signifie que le hook ne sera pas exécuté dans un environnement virtuel, mais directement sur le système de l'utilisateur.
