f = open("states_to_numbers", "r")
lines = f.readlines()
states_to_numbers = {}
for line in lines:
    states_to_numbers[line.split()[0]] = line.split()[1]

dfa_file = open("dfa.original")
dfa_lines = dfa_file.readlines()
dfa_list = []

for dfa_line in dfa_lines:
    #print(len(dfa_line))
    if dfa_line == '\n':
        continue
    a = states_to_numbers[dfa_line.split()[0]]
    b = dfa_line.split()[1]
    c = states_to_numbers[dfa_line.split()[2]]
    dfa_list.append((a, b, c))

output_file = open("dfa.converted", "w")
for d, e, f2 in dfa_list:
    output_file.write(d)
    output_file.write(" ")
    output_file.write(e)
    output_file.write(" ")
    output_file.write(f2)
    output_file.write("\n")

output_file.close()
f.close()
dfa_file.close()
