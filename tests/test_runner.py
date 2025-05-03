import pytest
import socket
import yaml

def load_cases(file_path):
    with open(file_path, 'r') as f:
        return yaml.safe_load(f)

@pytest.mark.parametrize("case", load_cases("cases/pass_nick_user.yml"), ids=lambda x: x["name"])
def test_case(case):
    sock = socket.create_connection(('localhost', 4242), timeout=3)
    for step in case["steps"]:
        sock.sendall((step["send"] + "\r\n").encode())

    recv_data = sock.recv(4096).decode()
    print("Received:", recv_data)  # デバッグ用
    assert case["expect"] in recv_data
    sock.close()
