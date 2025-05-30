import matplotlib.pyplot as plt
import numpy as np

# make data
x = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
y_16kib = list(map(lambda s: int(s, 16), ["0x26", "0x2b", "0x2b", "0x2b", "0x2b", "0x2b", "0x2b", "0x2b", "0x2b", "0x2b"]))
y_32kib = list(map(lambda s: int(s, 16), ["0x25", "0x2d", "0x31", "0x33", "0x35", "0x37", "0x39", "0x3b", "0x3d", "0x3f"]))
y_48kib = list(map(lambda s: int(s, 16), ["0x30", "0xcf", "0x16f", "0x209", "0x2aa", "0x337", "0x3c4", "0x451", "04d4", "0x561"]))

# plot
fig, ax = plt.subplots()
print(y_32kib)
ax.plot(x, y_16kib, '', linewidth=2, label='16 Kib Array Size')
ax.plot(x, y_32kib, '', markeredgewidth=2, label='32 Kib Array Size')
ax.plot(x, y_48kib, '', linewidth=2.0, label='48 Kib Array Size')
ax.set_yscale('log', base=2)
ax.set_ylabel('Number of L2 Cache Hits')
ax.set_xlabel('Number of Pass')
ax.legend()

plt.savefig('l2-hits.pdf')
#plt.show()
