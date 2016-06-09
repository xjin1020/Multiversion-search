import os, os.path

files = [name for name in os.listdir("./data")]
files.sort() #sort the list of strings alphabetically 
filenumber = len(files)
for i in range(1,filenumber+1):
    result_filename = "result"+str(i)
    #os.mkdir(result_filename)
    #command="time ./fragmentation1 "+"data/"+files[i-1]+" "+result_filename+"/"
    #print "command: "+command
    #output=os.popen(command).read()
    #a=output.split("\n")
    #for b in a:
    #    print b

    #command="time ./fragmentation2 "+"data/"+files[i-1]+" "+result_filename+"/"
    #print "command: "+command
    #output=os.popen(command).read()
    #a=output.split("\n")
    #for b in a:
    #    print b

    command="time ./fragmentation3 "+"data/"+files[i-1]+" "+result_filename+"/"
    print "command: "+command
    output=os.popen(command).read()
    a=output.split("\n")
    for b in a:
        print b
