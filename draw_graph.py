import os
import matplotlib.pyplot as p

s = 'text.txt'
if not os.path.exists(s):
  print('File', s, 'doesn\'t exist.')
  exit()
f = open(s)
a,b = [], []
for line in f:
  a.append(line.split()[0])
  b.append(line.split()[1])
f.close()

time = list(map(int, a))
beings_alive = list(map(int, b))

width = time[-1]
height = beings_alive[0]

fig, ax = p.subplots()
mng = p.get_current_fig_manager()
mng.full_screen_toggle()

fig.canvas.set_window_title('Population dynamics')
ax.axis([0,width,0,height])
ax.set_xlabel('Time')
ax.set_ylabel('Beings alive')
ax.plot(time,beings_alive,'k')
p.show()