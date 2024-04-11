import dataclasses
import json

CONFIGURATION_FILE_PATH = "/etc/smyt/conf.json"


@dataclasses.dataclass(frozen=True)
class Config:
    process_period: int
    warning_threshold: int
    panic_threshold: int
    device: str


def read_configuration() -> Config:
    try:
        with open(CONFIGURATION_FILE_PATH, "r") as file:
            configuration = json.load(file)
    except json.JSONDecodeError as err:
        print(f"Could not parse configuration file: {err}")
        return
    except FileNotFoundError as err:
        print(f"Could not find configuration file: {err}")
        return
    except OSError as err:
        print(f"Could not open configuration file for reading: {err}")
        return

    process_period = configuration["process_period"]  # TODO error
    warning_threshold = configuration["warning_threshold"]
    panic_threshold = configuration["panic_threshold"]
    device = configuration["device"]

    return Config(process_period, warning_threshold, panic_threshold, device)
