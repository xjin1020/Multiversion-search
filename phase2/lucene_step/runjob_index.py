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

def run_command_n2(com):
    os.popen(com)

fin = open('../convert_table.txt','r')
for line in fin.readlines():
    line_list = line.split(' ')
    did = line_list[0]
    print 'Lucene indexing cluster '+did
    run_command_n2('./run_index.sh ../cluster/'+did+'/fragments/')
    run_command_n('mv index/ index2/') # move to another name in case python write to a new index before finishing moving
    run_command_n('mv index2/ ../cluster/'+did+'/index')


