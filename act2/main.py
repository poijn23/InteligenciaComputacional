class Mapa:
    def __init__(self, ciudades):
        self.ciudades = ciudades
        self.camino = dict.fromkeys(self.ciudades,"")

        # Relaciona el nombre de cada ciudad con su posición.
        self.indices = {}

        for posicion in range(len(ciudades)):
            ciudad = ciudades[posicion]
            self.indices[ciudad] = posicion

        # Crear matriz llena de ceros.
        cantidad = len(ciudades)
        self.matriz = []

        for fila in range(cantidad):
            nueva_fila = [0] * cantidad
            self.matriz.append(nueva_fila)

    def conectar(self, ciudad1, ciudad2):
        fila = self.indices[ciudad1]
        columna = self.indices[ciudad2]

        # El mapa permite viajar en ambos sentidos.
        self.matriz[fila][columna] = 1
        self.matriz[columna][fila] = 1

    def obtener_vecinos(self, ciudad):
        vecinos = []
        fila = self.indices[ciudad]

        for columna in range(len(self.ciudades)):
            if self.matriz[fila][columna] == 1:
                vecinos.append(self.ciudades[columna])

        return vecinos

    def recorrer_camino(self,ciudad):
        vecinos = self.obtener_vecinos(ciudad)
        nuevos=[]
        for vecino in vecinos:
            if not self.camino[vecino]:
                self.camino[vecino]= ciudad
                nuevos.append(vecino)
        return nuevos

    def buscar_camino(self,ciudad,destino):
        cola = [ciudad]

        while cola:
            actual = cola.pop()
            if actual == destino:
                break
            nuevos = self.recorrer_camino(actual)
            cola.extend(nuevos)

        recorrido = [destino]
        next_nodo = destino
        while True:
            next_nodo=self.camino[next_nodo]
            if next_nodo == "origen":
                break
            recorrido.append(next_nodo)

        for nodo in recorrido:
            print(nodo)



    def mover(self, ciudad_actual, destino):
        fila = self.indices[ciudad_actual]
        columna = self.indices[destino]

        if self.matriz[fila][columna] == 1:
            return destino

        return None

    def mostrar_mapa(self):
        for ciudad in self.ciudades:
            vecinos = self.obtener_vecinos(ciudad)
            print(ciudad, "->", vecinos)
    def set_origin(self,ciudad_origen):
        self.camino[ciudad_origen]="origen"

ciudades = [
    "Arad",
    "Zerind",
    "Oradea",
    "Sibiu",
    "Timisoara",
    "Lugoj",
    "Mehadia",
    "Drobeta",
    "Craiova",
    "Rimnicu Vilcea",
    "Fagaras",
    "Pitesti",
    "Bucharest",
    "Giurgiu",
    "Urziceni",
    "Hirsova",
    "Eforie",
    "Vaslui",
    "Iasi",
    "Neamt"
]

mapa = Mapa(ciudades)

# Conexiones del mapa
mapa.conectar("Arad", "Zerind")
mapa.conectar("Arad", "Sibiu")
mapa.conectar("Arad", "Timisoara")

mapa.conectar("Zerind", "Oradea")
mapa.conectar("Oradea", "Sibiu")

mapa.conectar("Timisoara", "Lugoj")
mapa.conectar("Lugoj", "Mehadia")
mapa.conectar("Mehadia", "Drobeta")
mapa.conectar("Drobeta", "Craiova")

mapa.conectar("Sibiu", "Fagaras")
mapa.conectar("Sibiu", "Rimnicu Vilcea")

mapa.conectar("Rimnicu Vilcea", "Craiova")
mapa.conectar("Rimnicu Vilcea", "Pitesti")

mapa.conectar("Craiova", "Pitesti")

mapa.conectar("Fagaras", "Bucharest")
mapa.conectar("Pitesti", "Bucharest")

mapa.conectar("Bucharest", "Giurgiu")
mapa.conectar("Bucharest", "Urziceni")

mapa.conectar("Urziceni", "Hirsova")
mapa.conectar("Hirsova", "Eforie")

mapa.conectar("Urziceni", "Vaslui")
mapa.conectar("Vaslui", "Iasi")
mapa.conectar("Iasi", "Neamt")


def main():
    ciudad_actual = "Arad"
    mapa.set_origin(ciudad_actual)

    print("Ciudad inicial:", ciudad_actual)
    print("Ciudades disponibles:", mapa.obtener_vecinos(ciudad_actual))

    # Ejemplo de cómo desplazarse, sin resolver el ejercicio.
    destino = input("Escribe la ciudad a la que quieres moverte: ")

    nueva_ciudad = mapa.mover(ciudad_actual, destino)

    if nueva_ciudad is not None:
        ciudad_actual = nueva_ciudad
        print("Ahora estás en:", ciudad_actual)
    else:
        print("No existe una conexión directa con", destino)


if __name__ == "__main__":
    main()