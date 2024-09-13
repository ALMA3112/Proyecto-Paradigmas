

# Definimos una clase para representar los símbolos utilizados en la cinta
class Simbolo:
    BLANCO = ' '  # Representa una celda vacía en la cinta
    CERO = '0'    # Representa el dígito binario 0
    UNO = '1'     # Representa el dígito binario 1

# Clase para representar la configuración actual de la máquina de Turing
class Configuracion:
    def __init__(self, cinta, cabeza, estado):
        self.cinta = cinta    # La cinta de la máquina
        self.cabeza = cabeza  # El cabezal de lectura/escritura
        self.estado = estado  # El estado actual de la máquina

# Clase para manejar las tablas de transición de la máquina de Turing
class TablaTransiciones:
    def __init__(self, transiciones):
        self.transiciones = transiciones  # Lista de transiciones

    # Método para obtener la siguiente transición basada en el estado actual y el símbolo leído
    def obtener_transicion(self, estado, simbolo):
        indice_simbolo = 1 if simbolo == Simbolo.UNO else 0
        return self.transiciones[estado][indice_simbolo]

# Clase para representar la cinta de la máquina de Turing
class Cinta:
    def __init__(self, entrada, tamanio=200):
        self.tamanio = tamanio  # Tamaño total de la cinta
        # Inicializamos la cinta con espacios en blanco y la entrada
        self.cinta = [Simbolo.BLANCO] * (len(entrada) + self.tamanio)
        for i, char in enumerate(entrada):
            self.cinta[i] = char

    # Método para leer un símbolo de la cinta en una posición dada
    def leer(self, posicion):
        return self.cinta[posicion]

    # Método para escribir un símbolo en la cinta en una posición dada
    def escribir(self, posicion, simbolo):
        self.cinta[posicion] = simbolo

    # Método para representar la cinta como una cadena
    def __str__(self):
        return ''.join(self.cinta).strip()

# Clase para representar el cabezal de lectura/escritura
class Cabezal:
    def __init__(self, posicion_inicial=0):
        self.posicion = posicion_inicial  # Posición inicial del cabezal

    # Método para mover el cabezal
    def mover(self, direccion):
        self.posicion += direccion

    # Método para obtener la posición actual del cabezal
    def obtener_posicion(self):
        return self.posicion

# Clase principal de la Máquina de Turing
class MaquinaTuring:
    MAX_PASOS = 10000  # Número máximo de pasos para evitar bucles infinitos

    def __init__(self):
        # Definimos las tablas de transición para cada operación
        self.tabla_transiciones_suma = TablaTransiciones([
            [[0, Simbolo.CERO, 1], [0, Simbolo.UNO, 1]],
            [[1, Simbolo.CERO, -1], [2, Simbolo.UNO, -1]],
            [[2, Simbolo.UNO, -1], [1, Simbolo.CERO, -1]],
            [[1, Simbolo.UNO, -1], [2, Simbolo.CERO, -1]],
            [[3, Simbolo.BLANCO, 1], [3, Simbolo.BLANCO, 1]],
            [[4, Simbolo.BLANCO, 0], [4, Simbolo.BLANCO, 0]]
        ])

        self.tabla_transiciones_resta = TablaTransiciones([
            [[0, Simbolo.CERO, 1], [0, Simbolo.UNO, 1]],
            [[1, Simbolo.CERO, -1], [2, Simbolo.UNO, -1]],
            [[2, Simbolo.UNO, -1], [1, Simbolo.CERO, -1]],
            [[3, Simbolo.BLANCO, 1], [3, Simbolo.BLANCO, 1]],
            [[4, Simbolo.BLANCO, 0], [4, Simbolo.BLANCO, 0]]
        ])

        self.tabla_transiciones_multiplicacion = TablaTransiciones([
            [[0, Simbolo.CERO, 1], [1, Simbolo.CERO, 1]],
            [[1, Simbolo.CERO, 1], [2, Simbolo.UNO, -1]],
            [[2, Simbolo.UNO, -1], [1, Simbolo.CERO, -1]],
            [[3, Simbolo.CERO, -1], [2, Simbolo.UNO, -1]],
            [[4, Simbolo.BLANCO, 1], [4, Simbolo.BLANCO, 1]],
            [[5, Simbolo.BLANCO, 0], [5, Simbolo.BLANCO, 0]]
        ])

        self.tabla_transiciones_division = TablaTransiciones([
            [[0, Simbolo.CERO, 1], [0, Simbolo.UNO, 1]],
            [[1, Simbolo.CERO, -1], [2, Simbolo.UNO, -1]],
            [[2, Simbolo.UNO, -1], [1, Simbolo.CERO, -1]],
            [[3, Simbolo.CERO, 1], [3, Simbolo.UNO, 1]],
            [[4, Simbolo.UNO, -1], [4, Simbolo.CERO, -1]],
            [[5, Simbolo.BLANCO, 1], [5, Simbolo.BLANCO, 1]],
            [[6, Simbolo.BLANCO, 0], [6, Simbolo.BLANCO, 0]]
        ])

    # Método para inicializar la máquina con una entrada dada
    def inicializar_maquina(self, entrada):
        return Configuracion(Cinta(entrada), Cabezal(), 0)

    # Método para imprimir el estado actual de la cinta
    def imprimir_cinta(self, maquina):
        print("Cinta: ", end="")
        inicio = max(0, maquina.cabeza.obtener_posicion() - 20)
        fin = min(len(maquina.cinta.cinta), maquina.cabeza.obtener_posicion() + 21)
        for i in range(inicio, fin):
            if i == maquina.cabeza.obtener_posicion():
                print(f"[{maquina.cinta.leer(i) if maquina.cinta.leer(i) != Simbolo.BLANCO else '_'}]", end="")
            else:
                print(maquina.cinta.leer(i) if maquina.cinta.leer(i) != Simbolo.BLANCO else '_', end="")
        print(f"\nPosición de la cabeza: {maquina.cabeza.obtener_posicion()}, Estado: {maquina.estado}")

    # Método para realizar una operación en la máquina de Turing
    def realizar_operacion(self, maquina, tabla_transiciones, num_estados):
        pasos = 0
        while maquina.estado < num_estados - 1 and pasos < self.MAX_PASOS:
            simbolo_actual = maquina.cinta.leer(maquina.cabeza.obtener_posicion())
            if simbolo_actual == Simbolo.BLANCO:
                break

            nuevo_estado, nuevo_simbolo, direccion = tabla_transiciones.obtener_transicion(maquina.estado, simbolo_actual)

            maquina.cinta.escribir(maquina.cabeza.obtener_posicion(), nuevo_simbolo)
            maquina.cabeza.mover(direccion)
            maquina.estado = nuevo_estado

            pasos += 1

        if pasos >= self.MAX_PASOS:
            print("Advertencia: Se alcanzó el límite máximo de pasos.")

    # Método para interpretar y mostrar el resultado de la operación
    def interpretar_resultado(self, maquina, operacion, num1_decimal, num2_decimal):
        print("Resultado encontrado en la cinta:")
        resultado_binario = self.extraer_resultado_binario(maquina.cinta)
        if resultado_binario:
            resultado_decimal = int(resultado_binario, 2)
            print(f"Binario: {resultado_binario}")
            print(f"Decimal: {resultado_decimal}")
        else:
            print("No se encontraron números binarios válidos en la cinta.")

        resultado_esperado = Operacion.realizar(num1_decimal, num2_decimal, operacion)
        print(f"\nResultado esperado de la operación:")
        print(f"Decimal: {num1_decimal} {operacion} {num2_decimal} = {resultado_esperado}")
        print(f"Binario: {bin(resultado_esperado)[2:] if resultado_esperado is not None else 'N/A'}")

    # Método para extraer el resultado binario de la cinta
    def extraer_resultado_binario(self, cinta):
        resultado = ''
        encontrado_uno = False
        for simbolo in cinta.cinta:
            if simbolo in [Simbolo.CERO, Simbolo.UNO]:
                if simbolo == Simbolo.UNO:
                    encontrado_uno = True
                if encontrado_uno:
                    resultado += simbolo
        return resultado if resultado else None

# Clase para manejar las operaciones aritméticas
class Operacion:
    SUMA = '+'
    RESTA = '-'
    MULTIPLICACION = '*'
    DIVISION = '/'

    # Método para verificar si una operación es válida
    @staticmethod
    def es_valida(op):
        return op in [Operacion.SUMA, Operacion.RESTA, Operacion.MULTIPLICACION, Operacion.DIVISION]

    # Método para realizar la operación aritmética
    @staticmethod
    def realizar(num1, num2, op):
        if op == Operacion.SUMA:
            return num1 + num2
        elif op == Operacion.RESTA:
            return num1 - num2
        elif op == Operacion.MULTIPLICACION:
            return num1 * num2
        elif op == Operacion.DIVISION:
            return num1 // num2 if num2 != 0 else None

# Clase para manejar la entrada del usuario
class EntradaUsuario:
    # Método para obtener los números binarios del usuario
    @staticmethod
    def obtener_numeros_binarios():
        while True:
            entrada = input("Ingrese dos números binarios separados por un espacio: ")
            if EntradaUsuario.es_binario_valido(entrada):
                return entrada.split()
            print("Error: La entrada debe contener solo '0', '1' y espacios.")

    # Método para obtener la operación del usuario
    @staticmethod
    def obtener_operacion():
        while True:
            operacion = input("Ingrese la operación (+, -, *, /): ")
            if Operacion.es_valida(operacion):
                return operacion
            print("Error: Operación no reconocida.")

    # Método para validar si una cadena es un número binario válido
    @staticmethod
    def es_binario_valido(cadena):
        return all(char in '01 ' for char in cadena)

# Clase principal de la aplicación
class Aplicacion:
    def __init__(self):
        self.maquina_turing = MaquinaTuring()

    # Método principal para ejecutar la aplicación
    def ejecutar(self):
        num1, num2 = EntradaUsuario.obtener_numeros_binarios()
        operacion = EntradaUsuario.obtener_operacion()

        entrada = f"{num1} {num2}"
        maquina = self.maquina_turing.inicializar_maquina(entrada)

        print("Entrada inicial:")
        self.maquina_turing.imprimir_cinta(maquina)

        num1_decimal = int(num1, 2)
        num2_decimal = int(num2, 2)

        if operacion == Operacion.SUMA:
            self.maquina_turing.realizar_operacion(maquina, self.maquina_turing.tabla_transiciones_suma, 6)
        elif operacion == Operacion.RESTA:
            if num1_decimal < num2_decimal:
                print("Error: El primer número debe ser mayor o igual al segundo para la resta.")
                return
            self.maquina_turing.realizar_operacion(maquina, self.maquina_turing.tabla_transiciones_resta, 5)
        elif operacion == Operacion.MULTIPLICACION:
            self.maquina_turing.realizar_operacion(maquina, self.maquina_turing.tabla_transiciones_multiplicacion, 6)
        elif operacion == Operacion.DIVISION:
            if num2_decimal == 0:
                print("Error: División por cero.")
                return
            self.maquina_turing.realizar_operacion(maquina, self.maquina_turing.tabla_transiciones_division, 7)

        print(f"\nResultado de la operación {operacion}:")
        self.maquina_turing.imprimir_cinta(maquina)

        print("\nInterpretación del resultado:")
        self.maquina_turing.interpretar_resultado(maquina, operacion, num1_decimal, num2_decimal)

# Punto de entrada del programa
if __name__ == "__main__":
    app = Aplicacion()
    app.ejecutar()