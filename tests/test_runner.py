import pytest # type: ignore
import socket
import time
import yaml # type: ignore
import os
import glob

import builtins

SERVER_HOST = "127.0.0.1"
SERVER_PORT = getattr(builtins, "SERVER_PORT", 4242)
TIMEOUT = 2.0

RED = "\033[31m"
GREEN = "\033[32m"
CYAN = "\033[36m"
BOLDWHITE = "\033[1;37m"
RESET = "\033[0m"

class IRCClient:
    def __init__(self, name):
        self.name = name
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(TIMEOUT)
        self.sock.connect((SERVER_HOST, SERVER_PORT))

    def send(self, msg):
        if not msg.endswith("\r\n"):
            msg += "\r\n"
        self.sock.sendall(msg.encode())

    def receive(self):
        try:
            return self.sock.recv(4096).decode()
        except socket.timeout:
            return ""

    def close(self):
        self.sock.close()

def expand_repeat_case(case):
    if "repeat" not in case:
        return [case]

    expanded = []
    count = case["repeat"]
    for i in range(count):
        new_case = {
            "name": f"{case['name']} (#{i})",
            "steps": [],
            "expect": []
        }

        for step in case.get("steps", []):
            new_step = {}
            for k, v in step.items():
                new_step[k] = v.replace("{{i}}", str(i))
            new_step["client"] = f"client{i}"
            new_case["steps"].append(new_step)

        for exp in case.get("expect", []):
            new_exp = {}
            for k, v in exp.items():
                new_exp[k] = v.replace("{{i}}", str(i))
            new_exp["client"] = f"client{i}"
            new_case["expect"].append(new_exp)

        expanded.append(new_case)
    return expanded

def load_all_cases(directory):
    all_cases = []
    for path in glob.glob(os.path.join(directory, "*.yml")):
        with open(path) as f:
            content = yaml.safe_load(f)
            for case in content:
                expanded = expand_repeat_case(case)
                for expanded_case in expanded:
                    expanded_case["_source_file"] = os.path.basename(path)
                all_cases.extend(expanded)
    return all_cases

# ids=lambda case: ... で pytest の表示名をカスタマイズ
@pytest.mark.parametrize(
    "case",
    load_all_cases("cases"),
    ids=lambda case: case.get("name", case.get("_source_file", "unnamed_case"))
)
def test_case(case):
    clients = {}
    steps = case.get("steps", [])
    expects = case.get("expect", [])

    try:
        all_client_names = {s.get("client", "client1") for s in steps + expects}
        for name in all_client_names:
            clients[name] = IRCClient(name)

        for step in steps:
            client_name = step.get("client", "client1")
            if "send" in step:
                clients[client_name].send(step["send"])
                time.sleep(0.05)
            if "receive" in step:
                response = clients[client_name].receive()
                lines = response.splitlines()

                assert any(step["receive"] == line for line in lines), (
                    f"\n{CYAN}Test Name:{RESET} {case.get('name', 'Unnamed')}\n"
                    f"{CYAN}Source{RESET}:    {case.get('_source_file', '')}\n"
                    f"{CYAN}Client{RESET}:    {client_name}\n"
                    f"{CYAN}Command{RESET}:   '{step.get('send', '')}'\n"
                    f"{GREEN}Expected:  {BOLDWHITE}{step['receive']}{RESET}\n"
                    f"{RED}Received:  {BOLDWHITE}" + "\n".join(lines) + f"{RESET}\n"
                )

    finally:
        for client in clients.values():
            client.close()
