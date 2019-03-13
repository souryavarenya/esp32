with open('abc.txt', 'r+') as f:
    num = int(f.read())+235
    print(num)

with open('abc.txt', 'w') as f:
    f.write('%d' %(0))
    f.close()
