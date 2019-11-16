from subprocess import run, Popen
import sys
import os
#subprocess.run(["ls", "-l"])

causal_run =["./Causal "]
messages = ["Clock", "Multicast", "Commit"]
drift = ["50", "-20", "14"]
N = "4"

def run_program(args):
    os.system("ifconfig lo multicast")
    os.system("route add -net 224.0.0.0 netmask 240.0.0.0 dev lo")
    if(len(args) != 1):
        print("Please pass one of the option.")
        print("Supported options are: causal, noncausal, berkeley, bonus")
        exit(1)
    arg = args[0]
    if arg == "causal":
        for i in range(3):
            cmd = create_causal_command(i)
            #Popen(cmd, creationflags = CREATE_NEW_CONSOLE)
            os.system(" ".join(cmd))
    elif arg == "berkeley":
        for i in range(3):
            cmd = create_berkeley_command(i)
            os.system(" ".join(cmd))
        server_opt = ["./Berkeley", "-t", "0", "-s", "server"]
        os.system(" ".join(server_opt))
    elif arg == "noncausal":
        for i in range(3):
            cmd = create_noncausal_command(i)
            os.system(" ".join(cmd))
    elif arg == "bonus":
        for i in range(4):
            cmd = create_dme_command(i, N)
            os.system(" ".join(cmd))
    else:
    	print("invalid option. Only options allowed are causal, noncausal, berkeley, bonus")


def create_berkeley_command(i):
    command = []
    command.append("gnome-terminal -x")
    command.append("./Berkeley")
    command.append("-t")
    command.append(drift[i])
    return command

def create_causal_command(i):
    command = []
    command.append("gnome-terminal -x")
    command.append("./Causal")
    command.append("-i")
    command.append(str(i))
    command.append("-m")
    command.append(messages[i])
    return command

def create_noncausal_command(i):
    command = []
    command.append("gnome-terminal -x")
    command.append("./NonCausal")
    command.append("-i")
    command.append(str(i))
    command.append("-m")
    command.append(messages[i])
    return command

def create_dme_command(i, n):
    command = []
    command.append("gnome-terminal -x")
    command.append("./DistributedME")
    command.append("-i")
    command.append(str(i))
    command.append("-n")
    command.append(n)
    return command


if __name__ == "__main__":
    run_program(sys.argv[1:])