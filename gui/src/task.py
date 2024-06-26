from typing import Callable


class Task:
    def __init__(self, parent, action: Callable, delay: int):
        self._parent = parent
        self._action = action
        self._delay = delay
        self._id: str = None

    def start(self):
        self._id = self._parent.after(self._delay, self._call)

    def stop(self):
        if self._id is None:
            return

        self._parent.after_cancel(self._id)
        self._id = None

    def _call(self):
        if self._action():
            self._id = self._parent.after(self._delay, self._call)
