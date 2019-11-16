import subprocess
import sys
#subprocess.run(["ls", "-l"])

causal_run =["./Causal "]
messages = ["Clock", "Multicast", "Commit"]
drift = [50, -20, 14]
N = 4

def run_program(args):
    arg = args[0]
    if arg == "causal":
        for i in range(3):
            cmd = create_causal_command(i)
            subprocess.run(cmd, shell=True)
    elif arg == "berkeley":
        for i in range(3):
            cmd = create_berkeley_command(i)
            subprocess.run(cmd, shell=True)
        server_opt = ["./Berkeley", "-t", "0", "-s", "server"]
        subprocess.run(server_opt, shell=True)
    elif arg == "noncausal":
        for i in range(3):
            cmd = create_noncausal_command(i)
            subprocess.run(cmd, shell=True)
    elif args == "bonus":
        for i in range(4):
            cmd = create_dme_command(i, N)
            subprocess.run(cmd, shell=True)


def create_berkeley_command(i):
    command = []
    command.append("./Berkeley")
    command.append("-t")
    command.append(drift[i])
    return command

def create_causal_command(i):
    command = []
    command.append("./Causal")
    command.append("-i")
    command.append(i)
    command.append("-m")
    command.append(messages[i])
    return command

def create_noncausal_command(i):
    command = []
    command.append("./NonCausal")
    command.append("-i")
    command.append(i)
    command.append("-m")
    command.append(messages[i])
    return command

def create_dme_command(i, n):
    command = []
    command.append("./DistributedME")
    command.append("-i")
    command.append(i)
    command.append("-n")
    command.append(n)
    return command


if __name__ == "__main__":
    run_program(sys.argv[1:])