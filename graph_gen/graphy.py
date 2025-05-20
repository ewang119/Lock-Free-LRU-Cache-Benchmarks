import matplotlib
import matplotlib.pyplot

lf = open("lf_dump.txt", "r").read().strip().split("\n")
datapoints_get = {}
datapoints_put = {}

curr_threads = 64
datapoints_x = [1,2,3,4,8,16,32,64,128]
for l in lf:
    if "PARLAY_NUM_THREADS=" in l:
        curr_threads = int(l.strip().split("PARLAY_NUM_THREADS=")[1])
    elif "Time taken to put: " in l:
        datapoints_put[curr_threads] = 1/int(l.strip().split("Time taken to put: ")[1].split()[0]) * 10**9
    elif "Time taken to get: " in l:
        datapoints_get[curr_threads] = 1/int(l.strip().split("Time taken to get: ")[1].split()[0]) * 10**9

print(datapoints_put, datapoints_get)
matplotlib.pyplot.plot(datapoints_x, [datapoints_put[i] for i in datapoints_x], label="put", marker='+')
matplotlib.pyplot.ylim(bottom=0)
matplotlib.pyplot.xlabel("Number of Threads")
matplotlib.pyplot.ylabel("Mop/S")
matplotlib.pyplot.title("Lock Free Put Operations")
matplotlib.pyplot.savefig("put_lf.png")
matplotlib.pyplot.clf()
matplotlib.pyplot.plot(datapoints_x, [datapoints_get[i] for i in datapoints_x], label="get", marker='+')
matplotlib.pyplot.ylim(bottom=0)
matplotlib.pyplot.xlabel("Number of Threads")
matplotlib.pyplot.ylabel("Mop/S")
matplotlib.pyplot.title("Lock Free Get Operations")

matplotlib.pyplot.savefig("get_lf.png")
matplotlib.pyplot.clf()

datapoints_x = [1,4,16,64,128]

lf_get = []
lf_put = []

for i in datapoints_x:
    lf_get.append({"cacheSize": 100000, "threads": i, "numOps": 1000000, "zipf": .75, "res": datapoints_get[i]})
for i in datapoints_x:
    lf_put.append({"cacheSize": 100000, "threads": i, "numOps": 1000000, "zipf": .75, "res": datapoints_put[i]})



def grab_data(line):
    cacheSize = int(line[1])
    threads = int(line[2])
    numOps = int(line[3])
    zipf = float(line[4])
    mops = float(line[7])/1000
    return {"cacheSize": cacheSize, "threads": threads, "numOps": numOps, "zipf": zipf, "res": mops}

caffine_put = []
caffine_get = []
guava_put = []
guava_get = []

caffine_guava = open("new_results.txt", "r").read().strip().split("\n")
for l in caffine_guava:
    contents = l.split()
    if len(contents) == 0:
        continue
    elif contents[0] == "MyBenchmark.caffeineGet":
        caffine_get.append(grab_data(contents))
    elif contents[0] == "MyBenchmark.caffeinePut":
        caffine_put.append(grab_data(contents))
    elif contents[0] == "MyBenchmark.guavaGet":
        guava_get.append(grab_data(contents))
    elif contents[0] == "MyBenchmark.guavaPut":
        guava_put.append(grab_data(contents))

def grab_data_bl(line):
    _, num_elements, zipf, threads =  line[0].split("/")
    num_elements = int(num_elements)
    zipf = int(zipf)/100
    threads = int(threads.split(":")[1])
    mops = 1000000 * threads / float(line[1]) * 1000
    return {"cacheSize": num_elements, "numOps": 1000000, "zipf": zipf, "threads": threads, "res": mops}

bl_stuff = open("bl_dump.txt", "r").read().strip().split("\n")
bl_put = []
bl_get = []
for l in bl_stuff:
    contents = l.split()
    if len(contents) == 0:
        continue
    elif "BM_LRUCachePut" in contents[0]:
        bl_put.append(grab_data_bl(contents))
    elif "BM_LRUCacheGet" in contents[0]:
        bl_get.append(grab_data_bl(contents))

print(bl_put, bl_get)
print(lf_put, lf_get)

def get_rel_entries(datalist):
    filtered = {}
    for d in datalist:
        if d["cacheSize"] == 100000 and d["zipf"] == .75 and d["numOps"] == 1000000 and d["threads"] in datapoints_x:
            filtered[d["threads"]] = d["res"]
    return filtered

print(get_rel_entries(bl_put))
print(get_rel_entries(lf_put))
print(get_rel_entries(guava_put))
print(get_rel_entries(caffine_put))

matplotlib.pyplot.plot(datapoints_x, [get_rel_entries(bl_put)[i] for i in datapoints_x], label="Blocking", marker='+')
matplotlib.pyplot.plot(datapoints_x, [get_rel_entries(lf_put)[i] for i in datapoints_x], label="Lock Free", marker='+')
matplotlib.pyplot.plot(datapoints_x, [get_rel_entries(guava_put)[i] for i in datapoints_x], label="Guava", marker='+')
matplotlib.pyplot.plot(datapoints_x, [get_rel_entries(caffine_put)[i] for i in datapoints_x], label="Caffine", marker='+')
# matplotlib.pyplot.yscale("log")

matplotlib.pyplot.ylim(bottom=0)
matplotlib.pyplot.legend()
matplotlib.pyplot.xlabel("Number of Threads")
matplotlib.pyplot.ylabel("Mop/S")
matplotlib.pyplot.title("Put Operations")
matplotlib.pyplot.savefig("put.png")
matplotlib.pyplot.clf()
matplotlib.pyplot.plot(datapoints_x, [get_rel_entries(bl_get)[i] for i in datapoints_x], label="Blocking", marker='+')
matplotlib.pyplot.plot(datapoints_x, [get_rel_entries(lf_get)[i] for i in datapoints_x], label="Lock Free", marker='+')
matplotlib.pyplot.plot(datapoints_x, [get_rel_entries(guava_get)[i] for i in datapoints_x], label="Guava", marker='+')
matplotlib.pyplot.plot(datapoints_x, [get_rel_entries(caffine_get)[i] for i in datapoints_x], label="Caffine", marker='+')
# matplotlib.pyplot.yscale("log")

matplotlib.pyplot.ylim(bottom=0)
matplotlib.pyplot.legend()

matplotlib.pyplot.xlabel("Number of Threads")
matplotlib.pyplot.ylabel("Mop/S")
matplotlib.pyplot.title("Get Operations")
matplotlib.pyplot.savefig("get.png")
matplotlib.pyplot.clf()

