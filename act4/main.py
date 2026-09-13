import random

class Vector:
    def __init__(self, dimen):
        self.dimen=dimen
        self.VectorSphere = [random.uniform(-10,10) for i in range(dimen)]
        self.UpperVector = [10] * dimen
        self.LowwerVector= [-10] * dimen

    def reset(self):
        self.VectorSphere = [random.uniform(-10,10) for i in range(self.dimen)]

class Sphere:
    def __init__(self):
        self.count=0


    def formula (self, vector):
        return sum(xi*xi for xi in vector)

class main:

    def __init__(self):
        self.dim = 4
        self.p= 4
        self.r= .5
        self.call= 10
        self.sphere = Sphere()
        self.vector= Vector(self.dim)

    def solution_batch(self):
        self.sphere.count =0
        self.vector.reset()
        
        while self.sphere.count<self.call:
            copyVector= self.vector.VectorSphere.copy()
            for elem in range(len(copyVector)):
                if self.p > random.uniform(0,10):
                    while True:
                        nois=random.uniform(-self.r,self.r)
                        if self.vector.LowwerVector[elem]<=copyVector[elem]+nois<=self.vector.UpperVector[elem]:
                            break
                    copyVector[elem]= copyVector[elem]+nois

            if 0<=self.sphere.formula(copyVector)<=self.sphere.formula(self.vector.VectorSphere):
                self.vector.VectorSphere=copyVector
            self.sphere.count+=1

    def solution_one(self):
        self.sphere.count =0
        self.vector.reset()

        while self.sphere.count<self.call:
            copyVector= self.vector.VectorSphere.copy()
            for elem in range(len(copyVector)):
                nois=random.uniform(-self.r,self.r)
                copyVector[elem]=copyVector[elem]+nois
            if 0<=self.sphere.formula(copyVector)<=self.sphere.formula(self.vector.VectorSphere):
                self.vector.VectorSphere = copyVector
            self.sphere.count+=1

if __name__ == "__main__":
    m = main()

    m.solution_batch()
    print("Hill-Climbing con Bounded Uniform Convolution")
    print("  Vector:", m.vector.VectorSphere)
    print("  Sphere:", m.sphere.formula(m.vector.VectorSphere))

    m.solution_one()
    print("Hill-Climbing con ruido en todos los elementos")
    print("  Vector:", m.vector.VectorSphere)
    print("  Sphere:", m.sphere.formula(m.vector.VectorSphere))