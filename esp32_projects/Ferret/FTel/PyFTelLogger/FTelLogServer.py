import socket
import csv
import datetime

timestamp = datetime.datetime.now().strftime('%Y_%m_%d_%H_%M_%S')
dir = 'Logs/'

with open(dir + timestamp + '.csv', 'w',newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['TIMECODE', 'DATA1', 'DATA2'])
    f.close()

with open(dir + timestamp + '.txt','w') as m:
    m.write('%d' %(0))
    m.close()

# DATA_SIZE = 3
data_str = ""
# FORMAT <TIME><DATA1><DATA2>

# LOGS DATA INTO A CSV FILE AND RETURNS THE REMAINING UNPARSED STRING
def log_data(data_str, content, logfile, metafile):

    data_str += content.decode("utf-8")
    data_array = data_str.split(";")[:-1]
    data_str = data_str[data_str.rfind(';')+1:]
    # REFORMAT ARRAY
    data_array = [[float(x) for x in data_point.split(',')] for data_point in data_array]
    # LOGGING PART
    with open(logfile, 'a',newline='') as f:
        writer = csv.writer(f)
        writer.writerows(data_array)
        f.close()
    # DONE
    with open(metafile, 'r+') as m:
        num = int(m.read())+len(data_array)
        m.close()

    with open(metafile, 'w') as m:
        m.write('%d' %num)
        m.close()

    print("+1")
    return data_str

s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('0.0.0.0', 8888 ))
s.listen(0)

print("Starting FTel Logger...")

while True:

    client, addr = s.accept()

    while True:
        content = client.recv(4096)

        if len(content) == 0:
           break

        else:
            # print(content)
            data_str = log_data(data_str,content,dir + timestamp + '.csv',dir + timestamp + '.txt')

    print("Closing connection")
    client.close()
