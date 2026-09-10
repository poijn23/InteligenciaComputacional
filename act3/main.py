class node:
    def __init__(self,name,h):
        self.name= name
        self.g
        self.h=h
        self.f
        self.connected

    def setG(self,newG):
        self.g=newG
        self.getG(self.g,0)


    def getG(self,newG, cost):
        self.g= newG+cost
        self.f= self.g + self.h

class main:
    def __init__(self, points):
        self.points=points
        self.arist= dict.fromkeys(self.points,"")

        self.index={}

        for position in range(len(points)):
            point =points[position]
            self.index[point]=position

        cantidad = len(points)
        self.matriz=[]

        for fila in range(cantidad):
            nueva_fila= [0]*cantidad
            self.matriz.append(nueva_fila)

        H ={"S":5,
            "A":4,
            "B":2,
            "C":2,
            "D":1,
            "G":0} 
        
    def connetion(self, point1,point2,weight):
        fila = self.index[point1]
        columna = self.index[point2]

        self.matriz[fila][columna]= weight
        self.matriz[columna][fila]=weight

pointS = ["S",
          "A",
          "B",
          "D",
          "C",
          "G"]
        