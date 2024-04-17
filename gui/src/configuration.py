import dataclasses
import json

CONFIGURATION_FILE_PATH = "/etc/smyt/conf.json"


class ConfigError(RuntimeError):
    pass


@dataclasses.dataclass(frozen=True)
class Config:
    process_interval: int = 0
    warning_threshold: int = 0
    panic_threshold: int = 0
    device: str = ""


def read_configuration() -> Config:
    try:
        with open(CONFIGURATION_FILE_PATH, "r") as file:
            configuration = json.load(file)
    except json.JSONDecodeError as err:
        raise ConfigError(f"Could not parse configuration file: {err}")
    except FileNotFoundError as err:
        raise ConfigError(f"Could not find configuration file: {err}")
    except OSError as err:
        raise ConfigError(f"Could not open configuration file for reading: {err}")
    except Exception as err:
        raise ConfigError(f"Unexpected error reading configuration: {err}")

    try:
        process_interval = configuration["process_interval"]
        warning_threshold = configuration["warning_threshold"]
        panic_threshold = configuration["panic_threshold"]
        device = configuration["device"]
    except (KeyError, Exception) as err:
        raise ConfigError(f"Unexpected error reading configuration: {err}")

    return Config(process_interval, warning_threshold, panic_threshold, device)


def dump_configuration(config: Config) -> str:
    configuration = {}
    configuration["process_interval"] = config.process_interval
    configuration["warning_threshold"] = config.warning_threshold
    configuration["panic_threshold"] = config.panic_threshold
    configuration["device"] = config.device

    return json.dumps(configuration, indent=4)
