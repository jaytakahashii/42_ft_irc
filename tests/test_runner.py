import pytest # type: ignore
import socket
import yaml
import time
from threading import Thread

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 4242  # 適宜変更
TIMEOUT = 1.0

class IRCClient:
    def __init__(self, name):
        self.name = name
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(TIMEOUT)
        self.sock.connect((SERVER_HOST, SERVER_PORT))
        self.received = ""

    def send(self, message):
        if not message.endswith("\r\n"):
            message += "\r\n"
        self.sock.sendall(message.encode("utf-8"))

    def recv(self):
        try:
            data = self.sock.recv(4096)
            if not data:
                return None
            decoded = data.decode("utf-8")
            self.received += decoded
            return decoded
        except socket.timeout:
            return ""

    def close(self):
        self.sock.close()

    def get_all_messages(self):
        # 一定時間待機してからすべての受信を取得
        time.sleep(0.1)
        while True:
            msg = self.recv()
            if not msg:
                break
        return self.received


def load_cases(path):
    with open(path, "r") as f:
        return yaml.safe_load(f)


@pytest.mark.parametrize("case", load_cases("cases/authentication.yml"))
def test_case(case):
    clients = {}
    steps = case.get("steps", [])
    expects = case.get("expect", [])

    # デフォルトで1人だけのケースも想定
    if isinstance(expects, dict):
        expects = [expects]

    try:
        for step in steps:
            client_name = step.get("client", "client1")
            if client_name not in clients:
                clients[client_name] = IRCClient(client_name)

            if "send" in step:
                clients[client_name].send(step["send"])
            if "sleep" in step:
                time.sleep(step["sleep"])

        # 検証（各クライアントが受け取ったデータに expect が含まれているか）
        for expect in expects:
            if isinstance(expect, str):
                client_name = "client1"
                expected_msg = expect
            else:
                client_name = expect["client"]
                expected_msg = expect["receive"]

            actual = clients[client_name].get_all_messages()
            assert expected_msg in actual, f"[{client_name}] Expected:\n{expected_msg}\n\nGot:\n{actual}"

    finally:
        for client in clients.values():
            client.close()
