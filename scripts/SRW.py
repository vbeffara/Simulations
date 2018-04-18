from random import randint

l = 50000


def plot(w, s):
    f = open("%s.asy" % s, 'w')
    print("unitsize(2);", file=f)
    print("dot((0,0), linewidth(1));", file=f)
    while w != []:
        print("draw (%s);" % " -- ".join([str(z) for z in w[:100]]), file=f)
        w = w[99:]


W = [(0, 0)]

for i in range(l):
    (x, y) = W[-1]
    (dx, dy) = [(1, 0), (0, 1), (-1, 0), (0, -1)][randint(0, 3)]
    W.append((x+dx, y+dy))

plot(W, "SRW_len_%08d" % l)
