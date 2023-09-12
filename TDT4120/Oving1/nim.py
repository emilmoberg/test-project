
def take_pieces(n_pieces):
    rest = n_pieces % 8
    if (rest == 0):
        return 7
    elif (rest == 1):
        return 1
    return rest-1

print(take_pieces(12))