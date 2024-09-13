// Inclusión de bibliotecas necesarias
#include <stdio.h>    // Para funciones de entrada/salida estándar
#include <stdlib.h>   // Para funciones de asignación de memoria
#include <string.h>   // Para manipulación de cadenas
#include <math.h>     // Para funciones matemáticas (pow)

// Definición de constantes
#define BLANCO ' '         // Símbolo para representar una celda en blanco en la cinta
#define TAMANIO_CINTA 200  // Tamaño máximo de la cinta
#define MAX_ESTADOS 10     // Número máximo de estados en la máquina de Turing

// Definición de la estructura para la configuración de la máquina de Turing
typedef struct {
    char *cinta;   // Puntero a la cinta de la máquina
    int cabeza;    // Posición actual de la cabeza de lectura/escritura
    int estado;    // Estado actual de la máquina
} Configuracion;

// Tabla de transiciones para la operación de suma
// Cada entrada es de la forma {nuevo_estado, nuevo_simbolo, direccion}
int tabla_transiciones_suma[MAX_ESTADOS][2][3] = {
    {{0, '0', 1}, {0, '1', 1}},    // Estado 0: Mover a la derecha
    {{1, '0', -1}, {2, '1', -1}},  // Estado 1: Sumar 0 o 1
    {{2, '1', -1}, {1, '0', -1}},  // Estado 2: Propagar el acarreo
    {{1, '1', -1}, {2, '0', -1}},  // Estado 3: Propagar el acarreo (continuación)
    {{3, BLANCO, 1}, {3, BLANCO, 1}}, // Estado 4: Mover a la derecha hasta el final
    {{4, BLANCO, 0}, {4, BLANCO, 0}}  // Estado 5: Estado final
};

// Tabla de transiciones para la operación de resta
int tabla_transiciones_resta[MAX_ESTADOS][2][3] = {
    {{0, '0', 1}, {0, '1', 1}},    // Estado 0: Mover a la derecha
    {{1, '0', -1}, {2, '1', -1}},  // Estado 1: Restar 0 o 1
    {{2, '1', -1}, {1, '0', -1}},  // Estado 2: Propagar el préstamo
    {{3, BLANCO, 1}, {3, BLANCO, 1}}, // Estado 3: Mover a la derecha hasta el final
    {{4, BLANCO, 0}, {4, BLANCO, 0}}  // Estado 4: Estado final
};

// Tabla de transiciones para la operación de multiplicación
int tabla_transiciones_multiplicacion[MAX_ESTADOS][2][3] = {
    {{0, '0', 1}, {1, '0', 1}},    // Estado 0: Inicialización
    {{1, '0', 1}, {2, '1', -1}},   // Estado 1: Multiplicación por 0
    {{2, '1', -1}, {1, '0', -1}},  // Estado 2: Multiplicación por 1
    {{3, '0', -1}, {2, '1', -1}},  // Estado 3: Propagar el acarreo
    {{4, BLANCO, 1}, {4, BLANCO, 1}}, // Estado 4: Mover a la derecha hasta el final
    {{5, BLANCO, 0}, {5, BLANCO, 0}}  // Estado 5: Estado final
};

// Tabla de transiciones para la operación de división
int tabla_transiciones_division[MAX_ESTADOS][2][3] = {
    {{0, '0', 1}, {0, '1', 1}},    // Estado 0: Mover a la derecha
    {{1, '0', -1}, {2, '1', -1}},  // Estado 1: Restar el divisor
    {{2, '1', -1}, {1, '0', -1}},  // Estado 2: Propagar el préstamo
    {{3, '0', 1}, {3, '1', 1}},    // Estado 3: Mover a la derecha
    {{4, '1', -1}, {4, '0', -1}},  // Estado 4: Ajustar el resultado
    {{5, BLANCO, 1}, {5, BLANCO, 1}}, // Estado 5: Mover a la derecha hasta el final
    {{6, BLANCO, 0}, {6, BLANCO, 0}}  // Estado 6: Estado final
};

// Función para inicializar la máquina de Turing
void inicializar_maquina(Configuracion *maquina, const char *entrada) {
    int longitud = strlen(entrada);
    // Asignar memoria para la cinta
    maquina->cinta = (char *)malloc((longitud + TAMANIO_CINTA) * sizeof(char));
    if (maquina->cinta == NULL) {
        fprintf(stderr, "Error al asignar memoria para la cinta.\n");
        exit(EXIT_FAILURE);
    }
    // Inicializar la cinta con espacios en blanco
    memset(maquina->cinta, BLANCO, longitud + TAMANIO_CINTA);
    // Copiar la entrada a la cinta
    strncpy(maquina->cinta, entrada, longitud);
    maquina->cabeza = 0;  // Inicializar la posición de la cabeza
    maquina->estado = 0;  // Inicializar el estado
}

// Función para imprimir el estado actual de la cinta
void imprimir_cinta(const Configuracion *maquina) {
    printf("Cinta: ");
    // Determinar el rango de impresión
    int inicio = (maquina->cabeza > 20) ? maquina->cabeza - 20 : 0;
    int fin = (maquina->cabeza + 20 < TAMANIO_CINTA) ? maquina->cabeza + 20 : TAMANIO_CINTA - 1;

    // Imprimir la cinta
    for (int i = inicio; i <= fin; i++) {
        if (i == maquina->cabeza) {
            printf("[%c]", maquina->cinta[i] == BLANCO ? '_' : maquina->cinta[i]);
        } else {
            printf("%c", maquina->cinta[i] == BLANCO ? '_' : maquina->cinta[i]);
        }
    }
    printf("\n");
    printf("Posición de la cabeza: %d, Estado: %d\n", maquina->cabeza, maquina->estado);
}

// Función para mover la cabeza de lectura/escritura
void mover_cabeza(Configuracion *maquina, int direccion) {
    maquina->cabeza += direccion;
    // Asegurar que la cabeza no se salga de los límites de la cinta
    if (maquina->cabeza < 0) maquina->cabeza = 0;
    if (maquina->cabeza >= TAMANIO_CINTA) maquina->cabeza = TAMANIO_CINTA - 1;
}

// Función para escribir un símbolo en la posición actual de la cinta
void escribir(Configuracion *maquina, char simbolo) {
    maquina->cinta[maquina->cabeza] = simbolo;
}

// Función principal para realizar una operación en la máquina de Turing
void realizar_operacion(Configuracion *maquina, int tabla[][2][3], int num_estados) {
    int pasos = 0;
    const int MAX_PASOS = 10000;  // Límite de pasos para evitar bucles infinitos

    while (maquina->estado < num_estados - 1 && pasos < MAX_PASOS) {
        char simbolo_actual = maquina->cinta[maquina->cabeza];
        int indice_simbolo = (simbolo_actual == '1') ? 1 : (simbolo_actual == '0') ? 0 : -1;

        if (indice_simbolo == -1) break;  // Símbolo no reconocido

        // Obtener la siguiente acción de la tabla de transiciones
        int nuevo_estado = tabla[maquina->estado][indice_simbolo][0];
        char nuevo_simbolo = tabla[maquina->estado][indice_simbolo][1];
        int direccion = tabla[maquina->estado][indice_simbolo][2];

        // Aplicar la acción
        escribir(maquina, nuevo_simbolo);
        mover_cabeza(maquina, direccion);
        maquina->estado = nuevo_estado;

        pasos++;
    }

    if (pasos >= MAX_PASOS) {
        printf("Advertencia: Se alcanzó el límite máximo de pasos.\n");
    }
}

// Función para verificar si una cadena contiene solo '0', '1' y espacios
int es_binario_valido(const char *str) {
    while (*str) {
        if (*str != '0' && *str != '1' && *str != ' ') {
            return 0;
        }
        str++;
    }
    return 1;
}

// Función para convertir un número binario a decimal
int binario_a_decimal(const char* binario) {
    int decimal = 0;
    int potencia = 0;
    for (int i = strlen(binario) - 1; i >= 0; i--) {
        if (binario[i] == '1') {
            decimal += pow(2, potencia);
        }
        potencia++;
    }
    return decimal;
}

// Función para realizar operaciones aritméticas en decimal
int realizar_operacion_decimal(int num1, int num2, char operacion) {
    switch (operacion) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/': return (num2 != 0) ? num1 / num2 : -1;
        default: return 0;
    }
}

// Función para convertir un número decimal a binario
void decimal_a_binario(int decimal, char *binario) {
    int i = 0;
    if (decimal == 0) {
        binario[0] = '0';
        binario[1] = '\0';
        return;
    }
    while (decimal > 0) {
        binario[i++] = (decimal % 2) + '0';
        decimal = decimal / 2;
    }
    binario[i] = '\0';

    // Invertir la cadena
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = binario[start];
        binario[start] = binario[end];
        binario[end] = temp;
        start++;
        end--;
    }
}

// Función para verificar el estado de la máquina
void verificar_maquina(const Configuracion *maquina) {
    printf("\nVerificación de la máquina:\n");
    printf("Longitud de la cinta: %lu\n", strlen(maquina->cinta));
    printf("Posición de la cabeza: %d\n", maquina->cabeza);
    printf("Estado actual: %d\n", maquina->estado);

    if (maquina->cabeza < 0 || maquina->cabeza >= TAMANIO_CINTA) {
        printf("Error: La posición de la cabeza está fuera de los límites de la cinta.\n");
    }
    if (maquina->estado < 0 || maquina->estado >= MAX_ESTADOS) {
        printf("Error: El estado actual no es válido.\n");
    }
}

// Función para interpretar y mostrar el resultado de la operación
void interpretar_resultado(const Configuracion *maquina, char operacion, int num1_decimal, int num2_decimal) {
    int inicio = 0;
    int fin = 0;
    char numero_binario[TAMANIO_CINTA];
    int hay_numeros = 0;

    printf("Resultados en binario y decimal encontrados en la cinta:\n");

    // Buscar y mostrar números binarios en la cinta
    while (inicio < TAMANIO_CINTA) {
        while (inicio < TAMANIO_CINTA && maquina->cinta[inicio] == BLANCO) {
            inicio++;
        }

        if (inicio >= TAMANIO_CINTA) break;
        fin = inicio;
        while (fin < TAMANIO_CINTA && (maquina->cinta[fin] == '0' || maquina->cinta[fin] == '1')) {
            fin++;
        }

        int longitud = fin - inicio;
        if (longitud > 0) {
            strncpy(numero_binario, maquina->cinta + inicio, longitud);
            numero_binario[longitud] = '\0';

            int resultado_decimal = binario_a_decimal(numero_binario);
            printf("Binario: %s, Decimal: %d\n", numero_binario, resultado_decimal);
            hay_numeros = 1;
        }

        inicio = fin;
    }

    if (!hay_numeros) {
        printf("No se encontraron números binarios válidos en la cinta.\n");
    }

    // Mostrar el resultado de la operación en decimal
    int resultado_decimal = realizar_operacion_decimal(num1_decimal, num2_decimal, operacion);
    printf("\nResultado de la operación en decimal: %d %c %d = %d\n",
           num1_decimal, operacion, num2_decimal, resultado_decimal);

    // Convertir y mostrar el resultado en binario
    char resultado_binario[TAMANIO_CINTA];
    decimal_a_binario(resultado_decimal, resultado_binario);
    printf("Resultado de la operación en binario: %s\n", resultado_binario);

    // Verificar el estado final de la máquina
    verificar_maquina(maquina);
}

// ... [Código anterior se mantiene igual]

// Función principal
int main() {
    char entrada[TAMANIO_CINTA];
    char operacion;
    char num1[TAMANIO_CINTA/2], num2[TAMANIO_CINTA/2];

    // Leer la entrada del usuario
    printf("Ingrese dos números binarios separados por un espacio: ");
    if (fgets(entrada, TAMANIO_CINTA, stdin) == NULL) {
        printf("Error al leer la entrada.\n");
        return 1;
    }
    entrada[strcspn(entrada, "\n")] = '\0';  // Eliminar el salto de línea

    // Validar la entrada
    if (!es_binario_valido(entrada)) {
        printf("Error: La entrada debe contener solo '0', '1' y espacios.\n");
        return 1;
    }

    // Separar los dos números binarios de la entrada
    if (sscanf(entrada, "%s %s", num1, num2) != 2) {
        printf("Error: Debe ingresar dos números binarios.\n");
        return 1;
    }

    // Leer la operación a realizar
    printf("Ingrese la operación (+, -, *, /): ");
    if (scanf(" %c", &operacion) != 1) {
        printf("Error al leer la operación.\n");
        return 1;
    }

    // Inicializar la máquina de Turing
    Configuracion maquina;
    inicializar_maquina(&maquina, entrada);

    // Mostrar el estado inicial de la máquina
    printf("Entrada inicial:\n");
    imprimir_cinta(&maquina);

    // Convertir los números binarios a decimal para verificaciones
    int num1_decimal = binario_a_decimal(num1);
    int num2_decimal = binario_a_decimal(num2);

    // Realizar la operación seleccionada
    switch (operacion) {
        case '+':
            realizar_operacion(&maquina, tabla_transiciones_suma, MAX_ESTADOS);
            break;
        case '-':
            if (num1_decimal < num2_decimal) {
                printf("Error: El primer número debe ser mayor o igual al segundo para la resta.\n");
                free(maquina.cinta);
                return 1;
            }
            realizar_operacion(&maquina, tabla_transiciones_resta, MAX_ESTADOS);
            break;
        case '*':
            realizar_operacion(&maquina, tabla_transiciones_multiplicacion, MAX_ESTADOS);
            break;
        case '/':
            if (num2_decimal == 0) {
                printf("Error: División por cero.\n");
                free(maquina.cinta);
                return 1;
            }
            realizar_operacion(&maquina, tabla_transiciones_division, MAX_ESTADOS);
            break;
        default:
            printf("Error: Operación no reconocida.\n");
            free(maquina.cinta);
            return 1;
    }

    // Mostrar el resultado de la operación
    printf("\nResultado de la operación %c:\n", operacion);
    imprimir_cinta(&maquina);

    // Interpretar y mostrar el resultado en binario y decimal
    printf("\nInterpretación del resultado en binario y decimal:\n");
    interpretar_resultado(&maquina, operacion, num1_decimal, num2_decimal);

    // Liberar la memoria asignada para la cinta
    free(maquina.cinta);
    return 0;
}