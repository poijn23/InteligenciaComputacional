# Actividad 4 — Hill-Climbing con Bounded Uniform Convolution

Implementación del algoritmo **Hill-Climbing** para minimizar la función **Sphere**, usando dos formas distintas de generar el vecino (*Tweak*):

| Método | Tweak utilizado |
|---|---|
| `solution_batch()` | **Bounded Uniform Convolution** (Algoritmo 4): a cada elemento se le suma ruido con probabilidad `p`, respetando los límites `[L, U]`. |
| `solution_one()` | Ruido uniforme en **todos** los elementos, sin revisar límites. |

## Función objetivo

$$f(x) = \sum_{i=1}^{D} x_i^2$$

El mínimo global es `f(x) = 0` en `x = (0, 0, …, 0)`. Un vector nuevo se acepta si su valor de Sphere es **menor o igual** al del vector actual.

## Requisitos

- Python 3.x
- No requiere librerías externas; solo se usa el módulo estándar `random`.

## Ejecución

Desde la carpeta `act4`:

```bash
python main.py
```

En Windows también se puede usar el lanzador `py main.py`; en Linux/macOS, `python3 main.py`.

### Salida

El programa ejecuta ambos métodos, uno después del otro, e imprime el mejor vector encontrado y su valor de Sphere:

```text
Hill-Climbing con Bounded Uniform Convolution
  Vector: [x1, x2, x3, x4]
  Sphere: f(x)
Hill-Climbing con ruido en todos los elementos
  Vector: [x1, x2, x3, x4]
  Sphere: f(x)
```

Los valores cambian en cada ejecución, porque el vector inicial y el ruido son aleatorios y no se fija una semilla.

## Parámetros

Los parámetros del algoritmo se definen en el constructor de la clase `main` (`main.py`, líneas 24–27), y los límites en la clase `Vector` (líneas 7–8).

| Parámetro | Variable | Valor | Descripción | Se usa en |
|---|---|---|---|---|
| Dimensión `D` | `self.dim` | `4` | Número de elementos del vector solución. | Ambos |
| Probabilidad `p` | `self.p` | `4` | Probabilidad de agregar ruido a cada elemento. Se compara contra `random.uniform(0, 10)`, así que equivale a **p = 0.4 (40 %)**. | `solution_batch` |
| Medio rango `r` | `self.r` | `0.5` | El ruido se elige uniformemente en `[-r, r]`, es decir `[-0.5, 0.5]`. | Ambos |
| Condición de paro | `self.call` | `10` | Número de iteraciones del Hill-Climbing. En cada iteración se genera un vecino y se compara con el actual. | Ambos |
| Límite superior `U` | `UpperVector` | `[10] * D` | Valor máximo permitido por elemento. | `solution_batch` |
| Límite inferior `L` | `LowwerVector` | `[-10] * D` | Valor mínimo permitido por elemento. | `solution_batch` |
| Solución inicial `Xs` | `VectorSphere` | aleatorio | Cada elemento se genera con `random.uniform(-10, 10)`. | Ambos |

### Cómo se configuraron

- **`D = 4`**: una dimensión pequeña, para poder revisar el vector resultante a simple vista.
- **`p = 4` sobre una escala de 0 a 10 (40 %)**: en promedio se modifican 1 o 2 de los 4 elementos por iteración, así el vecino se parece al vector actual. Esto es lo que caracteriza a la búsqueda local.
- **`r = 0.5`**: pasos pequeños comparados con el rango `[-10, 10]` (5 % del medio rango). Evita saltos grandes que alejen al vecino del vector actual.
- **`call = 10`**: condición de paro por número de iteraciones. Con este valor el algoritmo hace pocas mejoras y normalmente termina lejos de 0. Para observar la convergencia hacia el mínimo conviene aumentarlo (por ejemplo a `1000`).
- **`L = -10`, `U = 10`**: dominio de búsqueda para la función Sphere.

### Cambiar los parámetros

Edita los valores en `main.__init__`:

```python
def __init__(self):
    self.dim = 4      # dimensión D
    self.p = 4        # probabilidad en escala 0–10 (4 = 40 %)
    self.r = .5       # medio rango del ruido
    self.call = 10    # número de iteraciones
```

Consideraciones:

- `p` se escribe en escala de **0 a 10**, no de 0 a 1: `p = 10` modifica siempre todos los elementos y `p = 0` no modifica ninguno.
- Si cambias los límites `UpperVector` / `LowwerVector`, cambia también el rango de `random.uniform(-10, 10)` en `Vector.__init__` y `Vector.reset` (líneas 6 y 11), para que el vector inicial quede dentro de los nuevos límites.

## Notas

- Cada método llama a `reset()` al iniciar: reinicia el contador de iteraciones y genera un vector inicial nuevo. Por eso cada método arranca desde un punto aleatorio distinto.
- El resultado de cada método queda guardado en `m.vector.VectorSphere`. La siguiente llamada lo sobrescribe, así que se debe leer (o copiar con `.copy()`) justo después de ejecutar cada método.
- `solution_one()` no revisa los límites, por lo que algún elemento podría salir del rango `[-10, 10]`.
