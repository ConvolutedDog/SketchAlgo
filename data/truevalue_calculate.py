file_list = ["pkt300w_test_3.txt"]
for file in file_list:
    packet_counter = {}
    pkt_list = []

    f = open(file)
    line = f.readline()
    row = 0
    while line:
        pkt = list(line.split())
        pkt_list.append(pkt[0])
        line = f.readline()
    f.close()

    with open("destination_before_integrate_" + file, "w") as f:
        f.write(str(len(pkt_list)))
        f.write("\n")
        for i in range(len(pkt_list)):
            f.write(pkt_list[i])
            f.write("\n")
    f.close()

    for item in pkt_list:
        if item not in packet_counter:
            packet_counter[item] = 1
        else:
            packet_counter[item] += 1

    with open("destination_after_integrate" + file, "w") as f:
        f.write(str(len(packet_counter)))
        f.write("\n")
        for key in packet_counter:
            f.write(key)
            f.write("\n")
        for value in packet_counter.values():
            f.write(str(value))
            f.write("\n")
    f.close()
