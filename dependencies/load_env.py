import os
from abc import ABC, abstractmethod
from typing import Any, Optional

from dotenv import load_dotenv


class EnvironmentType:
    LOCAL = 'LOCAL'
    AWSSSM = 'AWSSSM'


class BaseEnvironmentLoader(ABC):

    def __init__(self):
        ...

    @abstractmethod
    def load_env(self, key: str, default: Optional[str] = None) -> Any:
        pass


class LocalEnvLoader(BaseEnvironmentLoader):

    def __init__(self):
        super().__init__()
        load_dotenv()

    def load_env(self, key: str, default: Optional[str] = None) -> Any:
        return os.environ.get(key, default)


class EnvironmentLoaderFactory():

    def __init__(self):
        self.repository = {EnvironmentType.LOCAL: LocalEnvLoader}

    def get_environment_loader(self, type: str) -> BaseEnvironmentLoader:
        loader_class = self.repository.get(type)
        if loader_class:
            return loader_class()
        else:
            raise ValueError(f"Unknown environment type: {type}")
