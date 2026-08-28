import sys


def resultado(jugada_usuario):
    gana_a = {
        'piedra': 'papel',
        'papel': 'tijera',
        'tijera': 'piedra',
    }
    return gana_a[jugada_usuario]


def main():
    jugadas = ['piedra', 'papel', 'tijera']

    jugada_usuario = input("Juega piedra, papel o tijera: ").strip().lower()

    if jugada_usuario not in jugadas:
        print("Jugada inválida. Debe ser piedra, papel o tijera.")
        return

    jugada_maquina = resultado(jugada_usuario)
    print(f"La máquina jugó: {jugada_maquina}")

    if jugada_maquina == jugada_usuario:
        print("Empate")
    else:
        print("La máquina ganó")


if __name__ == "__main__":
    main()