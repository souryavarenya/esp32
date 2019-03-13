#! python3
import os
import time
#---
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
#---
import pandas as pd
#---

dir = 'Logs/'
ext = 'csv'

X_RANGE = 1000

#---

def retreive_file(dir,ext):
    flist = [f for f in [f for f in os.listdir(dir) if f.endswith('.' + ext)]]
    mlist = [b[5]+60*b[4]+3600*b[3]+86400*b[2]+2678400*b[1]+32140800*(b[0]-2019) for b in [[int(x) for x in a.split('.')[0].split('_')] for a in flist]]
    mindex = mlist.index(max(mlist))
    timestamp = flist[mindex].split('.')[0]
    return timestamp

latest = retreive_file(dir,ext)
# latest = ''
print(latest)
logfile = dir+latest+'.csv'
metafile = dir+latest+'.txt'

#---

style.use('fivethirtyeight')
fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)
ax1.set_ylim([-11,11])

def animate(i):

    while True:
        try:
            with open(metafile, 'r') as m:
                skipnum = int(float(m.read()) - X_RANGE)
                m.close()
                break
        except ValueError:
            pass

    if skipnum <= 0:
        skipnum = 0
    fields = ['TIMECODE','DATA1']
    out = pd.read_csv(logfile,skiprows = range(1,skipnum), usecols=fields)
    data = [out[a].tolist() for a in fields]
    print(len(data[0]))
    ax1.clear()
    ax1.plot(data[0][-X_RANGE:],data[1][-X_RANGE:])
    ax1.set_ylim([-12,12])

#---

ani = animation.FuncAnimation(fig, animate, interval=25)
plt.show()
