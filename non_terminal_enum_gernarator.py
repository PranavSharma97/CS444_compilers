f = open("non_term_state.txt","r")
lines = f.readlines()
counter = 1001
output_file = open("non_term_state_num.txt","w")
switch_file = open("token_display_name.txt","w")
for line in lines:
    token_name = line.split('\n')[0]
    output_file.write(token_name + " = " + str(counter) + ",\n")
    switch_file.write("case " + str(counter) +":\n m_display_name = \""+token_name + "\";\n break;\n")
    counter += 1
output_file.close()
switch_file.close()
f.close()
