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
        print "finish lucene_step for "+did+ " clusters"
    os.chdir(did+'/')
    #run_command_n('cp ../../lucene_step/binary_index_gen ./')
    run_command_n('../../lucene_step/binary_index_gen --index index < all_fragments.txt')
    os.chdir('..')

