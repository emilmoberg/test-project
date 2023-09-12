def merge(A, p, q, r):
    n1 = q - p + 1
    n2 = r - q

    left = [0] * (n1 + 1)
    right = [0] * (n2 + 1)

    for i in range(n1):
        left[i] = A[p + i]
    
    for j in range(n2):
        right[j] = A[q + j + 1]

    left[n1] = float('inf')
    right[n2] = float('inf')

    i = 0
    j = 0

    for k in range(p, r + 1):
        if left[i] <= right[j]:
            A[k] = left[i]
            i += 1
        else:
            A[k] = right[j]
            j += 1


def merge_sort(A, p, r):
    if p < r:
        q = (p + r)//2
        merge_sort(A, p, q)
        merge_sort(A, q + 1, r)
        merge(A, p, q, r)

A = [1,8,3,4,1,2,7,6,8]
merge(A,2,3,6)
print(A)