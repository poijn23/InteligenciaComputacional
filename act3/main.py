INFINITO = float("inf")


class node:
    def __init__(self, name, h):
        self.name = name
        self.g = INFINITO
        self.h = h
        self.f = INFINITO
        self.connected = []

    def setG(self, newG):
        self.g = newG
        self.f = self.g + self.h

    def getG(self, cost):
        return self.g + cost


class main:
    def __init__(self, points):
        self.points = points
        self.arist = dict.fromkeys(self.points, "")

        self.index = {}

        for position in range(len(points)):
            point = points[position]
            self.index[point] = position

        cantidad = len(points)
        self.matriz = []

        for fila in range(cantidad):
            nueva_fila = [0] * cantidad
            self.matriz.append(nueva_fila)

        self.H = {"S": 5,
                  "A": 4,
                  "B": 2,
                  "C": 2,
                  "D": 1,
                  "G": 0}

        self.nodes = {}
        for point in self.points:
            self.nodes[point] = node(point, self.H[point])

    def connetion(self, point1, point2, weight):
        fila = self.index[point1]
        columna = self.index[point2]

        self.matriz[fila][columna] = weight
        self.matriz[columna][fila] = weight

        self.nodes[point1].connected.append(point2)
        self.nodes[point2].connected.append(point1)

    def obtener_vecinos(self, point):
        vecinos = []
        fila = self.index[point]

        for columna in range(len(self.points)):
            peso = self.matriz[fila][columna]
            if peso > 0:
                vecinos.append((self.points[columna], peso))

        return vecinos

    def reiniciar(self):
        self.arist = dict.fromkeys(self.points, "")
        for point in self.points:
            self.nodes[point].g = INFINITO
            self.nodes[point].f = INFINITO

    def mejor_nodo(self, abiertos):
        mejor = abiertos[0]
        for point in abiertos:
            if self.nodes[point].f < self.nodes[mejor].f:
                mejor = point
        return mejor

    def reconstruir_camino(self, origen, destino):
        recorrido = [destino]
        siguiente = destino

        while siguiente != origen:
            siguiente = self.arist[siguiente]
            recorrido.append(siguiente)

        recorrido.reverse()
        return recorrido

    def buscar_camino_A_estrella(self, origen, destino):
        self.reiniciar()

        self.nodes[origen].setG(0)
        self.arist[origen] = origen

        abiertos = [origen]
        cerrados = []
        paso = 0

        while abiertos:
            paso = paso + 1
            actual = self.mejor_nodo(abiertos)

            print("Paso", paso, "-> lista abierta:", self.mostrar_lista(abiertos))
            print("         se expande:", actual,
                  "( g =", self.nodes[actual].g,
                  ", h =", self.nodes[actual].h,
                  ", f =", self.nodes[actual].f, ")")

            if actual == destino:
                camino = self.reconstruir_camino(origen, destino)
                return camino, self.nodes[destino].g

            abiertos.remove(actual)
            cerrados.append(actual)

            for vecino, peso in self.obtener_vecinos(actual):
                if vecino in cerrados:
                    continue

                tentativo = self.nodes[actual].getG(peso)

                if tentativo < self.nodes[vecino].g:
                    self.arist[vecino] = actual
                    self.nodes[vecino].setG(tentativo)

                    if vecino not in abiertos:
                        abiertos.append(vecino)

                    print("           vecino", vecino,
                          "-> g =", self.nodes[vecino].g,
                          ", f =", self.nodes[vecino].f,
                          ", padre =", actual)

        return None, INFINITO

    def mostrar_lista(self, abiertos):
        texto = []
        for point in abiertos:
            texto.append(point + "(f=" + str(self.nodes[point].f) + ")")
        return ", ".join(texto)

    def mostrar_mapa(self):
        for point in self.points:
            print(point, "->", self.obtener_vecinos(point))


pointS = ["S",
          "A",
          "B",
          "D",
          "C",
          "G"]

grafo = main(pointS)

# Aristas del grafo con su costo
grafo.connetion("S", "A", 1)
grafo.connetion("S", "B", 5)
grafo.connetion("A", "C", 2)
grafo.connetion("A", "D", 5)
grafo.connetion("B", "D", 1)
grafo.connetion("D", "C", 1)
grafo.connetion("D", "G", 2)
grafo.connetion("C", "G", 5)


def ejecutar():
    origen = "S"
    destino = "G"

    print("Grafo:")
    grafo.mostrar_mapa()

    print("\nBusqueda A* de", origen, "a", destino, "\n")
    camino, costo = grafo.buscar_camino_A_estrella(origen, destino)

    if camino is None:
        print("\nNo existe un camino entre", origen, "y", destino)
        return

    print("\nRuta encontrada:", " --> ".join(camino))
    print("Costo total:", costo)


if __name__ == "__main__":
    ejecutar()
