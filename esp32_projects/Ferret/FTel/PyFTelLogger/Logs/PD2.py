import pandas as pd

fields = ['TIMECODE','DATA1']
out = pd.read_csv('2019_03_11_17_22_40.csv',skiprows = range(1,1000))
# , usecols=fields

print(list(out))
