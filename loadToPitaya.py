"""
loadToPitaya.py

This script establishes an SSH connection to a specified device and copies files from the local 
machine to the remote machine using SCP (Secure Copy Protocol). It then executes a series of 
commands (specified in remote_commands) on the remote machine.

Note: Change the local_path !!!

"""

import subprocess
from paramiko import SSHClient
from scp import SCPClient

# SCP file paths
local_path = 'd:\\Red-Pitaya\\example'
remote_path = '/root'
scp_password = 'root'

# SSH command
ssh_device = 'rp-f0aa7c.local'
ssh_password = 'root'

# Remote commands
remote_commands = [
  # Remove example folder in /opt/redpitaya/www/apps
  'cd /opt/redpitaya/www/apps && rm -rf example',
  # Copy example folder from /root to /opt/redpitaya/www/apps
  'cd /opt/redpitaya/www/apps && cp -r /root/example ./',
  # List all files in /opt/redpitaya/www/apps/example
  'cd example && ls',
  # Make example app
  'cd example && make INSTALL_DIR=/opt/redpitaya'
]

# Run command in cmd
def run_cmd(cmd):
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    output, error = process.communicate()
    return output, error

# SSH connection
print('SSH connection to ', ssh_device)
ssh = SSHClient()
ssh.load_system_host_keys()
ssh.connect('rp-f0aa7c.local', username='root', password=ssh_password, look_for_keys=False)

# Copy files over SCP
print("SCP from ", local_path, " to ", remote_path)
with SCPClient(ssh.get_transport()) as scp:
        scp.put(local_path, recursive=True, remote_path=remote_path)

# Run remote commands
for cmd in remote_commands:
    print('Running: ', cmd)
    stdin, stdout, stderr = ssh.exec_command(cmd)
    print('Output gibberish: ', stdout.read().decode(), stderr.read().decode())

# Close SSH and SCP connections
ssh.close()
scp.close()

print('SSH and SCP connections closed')
