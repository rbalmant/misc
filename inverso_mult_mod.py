# Algoritmo euclidiano estendido
def aee(a, b):
    r = -1
    # Iremos mover os elementos da lista para a esquerda. De forma que, quando entramos em uma nova celula da tabela, o elemento mais a esquerda na lista (o primeiro) e perdido. Faremos isso para o quociente, os valores de x e de y.
    q = [0, 0]
    x = [1, 0]
    y = [0, 1]
    aux = 0

    # Inicialmente printamos os valores digitados na tabela (primeira celula)
    print a, "-", x[0], y[0]
    print b, "-", x[1], y[1]
    
    while r != 0:
        q[0] = q[1]
        q[1] = a / b
        
        aux = x[1]
        # x(n) = x(n-2) - x(n-1) * q(n)
        x[1] = x[0] - (x[1] * q[1])
        x[0] = aux
        
        aux = y[1]
        # y(n) = y(n-2) - y(n-1) * q(n)
        y[1] = y[0] - (y[1] * q[1])
        y[0] = aux
        
        r = a % b
        
        a = b
        b = r
        
        # Temos que ter uma condicao especial para r==0 pois nessa celula o exercicio nao quer x ou y
        if r == 0:
            print r, q[1], "-", "-"
        else:
            print r, q[1], x[1], y[1]
    
    return [a, x[0]]
    

n = input("")

while n > 0:
    a, b = input("")
    
    res = aee(a, b)
    if res[0] == 1:
        while (res[1] < 0):
            res[1] = res[1] + b
        print res[1]
    else:
        print "0"
    print "---"
    n = n - 1