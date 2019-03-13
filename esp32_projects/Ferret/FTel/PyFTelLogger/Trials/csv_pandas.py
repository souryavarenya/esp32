import pandas as pd

fields = ['col1','col3']

out = pd.read_csv('abc.csv', usecols=fields)
# ,skiprows = 2

print(out["col1"].tolist())
