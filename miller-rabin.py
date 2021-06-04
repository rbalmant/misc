def forma_reduzida(x, n):
    return x % n

def potencia_mod(e, a, mod):
    r = 1
    while (e > 0):
        eh_impar = e & 1
        if eh_impar:
            print r, a, e, "S"
            r = forma_reduzida(r * a, mod)
            a = forma_reduzida(a * a, mod)
            e = (e-1) / 2
        else:
            print r, a, e, "N"
            a = forma_reduzida(a*a, mod)
            e = e / 2
    print r, a, 0, "N"
    return r

f = input("")

while f > 0:
    n, b = input("")
    q = n - 1
    k = 0
    i = 0
    r = 0

    while ((q % 2) == 0):
        k = k + 1
        q = q / 2

    print k, q

    r = potencia_mod(q, b, n)
    


    while (i < k):
        print q, r
        if (i == 0 and r == 1) or (i >= 0 and r == (n - 1)):
            print "INCONCLUSIVO"
            print "---"
            break
        i += 1
        r = forma_reduzida(r*r, n)
        q = q*2
    if (i >= k):
        print "COMPOSTO"
        print "---"
    f -= 1