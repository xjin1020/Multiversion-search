import os

# function to run shell command
def run_command(com):
    print com
    output = os.popen(com).read()
    a = output.split('\n')
    for b in a:
        print b

# function to run shell command but no output to screen
def run_command_n(com):
    newc = com + ' >> log.out 2>&1'
    os.popen(newc)

fin = open('../convert_table.txt','r')
os.chdir('../cluster/')

for line in fin.readlines():
    line_list = line.split(' ')
    did = line_list[0]
    if (int(did) % 100 == 0):
        print "finish gen_index for "+did+ " clusters"
    os.chdir(did+'/')
    run_command_n('rm -rf fragments')
    run_command_n('rm all_fragments.txt')
    run_command_n('mkdir fragments')
    run_command_n('cp ../../gen_index/gen_index ./')
    run_command_n('./gen_index '+line_list[1])
    os.chdir('..')

