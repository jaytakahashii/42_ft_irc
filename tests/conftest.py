# conftest.py
def pytest_addoption(parser):
    parser.addoption(
        "--port", action="store", default="4242", help="IRC server port (default: 4242)"
    )

def pytest_configure(config):
    import builtins
    builtins.SERVER_PORT = int(config.getoption("--port"))
