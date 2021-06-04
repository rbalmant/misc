n = input("")

# Apenas para saber o indice de n2
original_crive = range(3, n, 2)

crive = range(3, n, 2)
index = 0
remove = []
n2 = 0
n_int_sqrt = int(n**(0.5))

print original_crive
print n_int_sqrt

while crive[index] <= n_int_sqrt:
    n2 = crive[index] * crive[index]
    print crive[index], n2, original_crive.index(n2)
    
    remove = []
    while n2 <= n:
        remove.append(n2)
        if n2 in crive:
            crive.remove(n2)
        n2 += (crive[index]*2)
        
    print remove
    print crive
    
    index += 1

# Incluimos o 2 tambem
print [2] + crive