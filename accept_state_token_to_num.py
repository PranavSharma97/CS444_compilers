token_f = open("token.translate","r")
token_lines = token_f.readlines()
token_num = {}
for line in token_lines:
    token_num[line.split()[0]] = line.split()[1]

dfa_f = open("dfa.translate","r")
dfa_lines = dfa_f.readlines()
dfa_num = {}
for line in dfa_lines:
    dfa_num[line.split()[0]] = line.split()[1]
    
a_t_original = open("accept_state.original")
a_t_lines = a_t_original.readlines()
a_t_list = []

for line in a_t_lines:
    if line == '\n':
        continue
    dfa = dfa_num[line.split()[0]]
    token = token_num[line.split()[1]]
    a_t_list.append((dfa,token))


output = open("accept_state.in","w")
for d,t in a_t_list:
    output.write(d+" "+t+"\n")

output.close()
dfa_f.close()
token_f.close()
a_t_original.close()
