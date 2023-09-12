#!/usr/bin/env python
# -*- coding: utf-8 -*-
import random


# De lokale testene består av to deler. Et sett med hardkodete
# instanser som kan ses lengre nede, og muligheten for å generere
# tilfeldig instanser. Genereringen av de tilfeldige instansene
# kontrolleres ved å juste på verdiene under.

# Kontrollerer om det genereres tilfeldige instanser.
generate_random_tests = False
# Antall tilfeldige tester som genereres
random_tests = 10
# Lavest mulig antall verdier i generert instans.
n_lower = 3
# Høyest mulig antall verdier i generert instans.
n_upper = 10
# Om denne verdien er 0 vil det genereres nye instanser hver gang.
# Om den er satt til et annet tall vil de samme instansene genereres
# hver gang, om verdiene over ikke endres.
seed = 0


def find_maximum_2(x):
    print("L: ", x)
    # ga nedover
    max = 0
    rising = False
    for i in range(1,len(x)):
        print("i: ", i)
        if x[i-1] > x[i]:
            print(x[i-1], ">", x[i])
            if x[i-1] > max:
                max = x[i-1]
            if (rising):
                break
        else:
            rising = True
    return max

def find_maximum(x):

    # flytt til slutt hvis større    
    if (len(x)>1):
        while x[0] > x[1] or x[0] > x[len(x)-1]:
            x.append(x.pop(0))

    print(x)

    left = 0
    right = len(x) - 1

    while left < right:
        mid = (left + right) // 2

        if x[mid] < x[mid + 1]:
            # Maksimum må være i høyre halvdel
            left = mid + 1
        else:
            # Maksimum må være i venstre halvdel (inkludert mid)
            right = mid

    # left peker nå på det største elementet
    return x[left]

# Hardkodete tester på format: (x, svar)
tests = [
    ([1], 1),
    ([1, 3], 3),
    ([3, 1], 3),
    ([1, 2, 1], 2),
    ([1, 0, 2], 2),
    ([2, 0, 1], 2),
    ([0, 2, 1], 2),
    ([0, 1, 2], 2),
    ([2, 1, 0], 2),
    ([2, 3, 1, 0], 3),
    ([2, 3, 4, 1], 4),
    ([2, 1, 3, 4], 4),
    ([4, 2, 1, 3], 4),
    ([42, 45, 7, 8, 12, 13, 26], 45),
]


# Genererer tilfeldige instanser med svar
def generate_examples(k, nl, nu):
    for _ in range(k):
        n = random.randint(nl, nu)
        x = random.sample(range(5*n), k=n)
        answer = max(x)
        t = x.index(answer)
        x = sorted(x[:t]) + [answer] + sorted(x[t + 1:], reverse=True)
        t = random.randint(0, n)
        x = x[t:] + x[:t]
        yield x, answer


if generate_random_tests:
    if seed:
        random.seed(seed)

    tests.extend(generate_examples(random_tests, n_lower, n_upper))


failed = False
for x, answer in tests:
    student = find_maximum(x[:])
    if student != answer:
        if failed:
            print("-"*50)

        failed = True

        print(f"""
Koden ga feil svar for følgende instans:
x: {x}

Ditt svar: {student}
Riktig svar: {answer}
""")

if not failed:
    print("Koden ga riktig svar for alle eksempeltestene")