def padint(input):
    input = str(input)
    pad = (len(input)%2) * '0'
    return pad + input