import csv
import datetime

timestamp = datetime.datetime.now().strftime('%Y_%m_%d_%H_%M_%S')

data = [['Spam', 'Lovely Spam', 'Wonderful Spam'],['Tram', 'Lovely Spam', 'Wonderful Spam'],['Scam', 'Lovely Spam', 'Wonderful Spam']]

with open('abc.csv', 'a',newline='') as f:
    writer = csv.writer(f)
    writer.writerows(data)
