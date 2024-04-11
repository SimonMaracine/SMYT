import subprocess

import tkinter as tk
import tkinter.ttk as ttk


class Smyt(tk.Frame):
    LOG_FILE_PATH = "/var/log/smyt/smyt.log"

    def __init__(self, root: tk.Tk):
        super().__init__(root)

        self._root = root
        self._logs = None

        self._configure()
        self._read_contents_log_file()

    def _configure(self):
        self.pack(fill="both", expand=True, padx=20, pady=20)
        self.columnconfigure(0, weight=0)
        self.columnconfigure(1, weight=2)
        self.rowconfigure(0, weight=1)

        self._root.title("SMYT")
        self._root.geometry("768x432")
        self._root.protocol("WM_DELETE_WINDOW", self._on_window_closed)

        self._configure_left_side()
        self._configure_contents()

    def _configure_left_side(self):
        frm_left_side = tk.Frame(self, relief="solid", borderwidth=1, padx=25, pady=20)
        frm_left_side.grid(row=0, column=0, sticky="nsw")

        frm_left_side.columnconfigure(0, weight=1)
        frm_left_side.rowconfigure(0, weight=1)
        frm_left_side.rowconfigure(1, weight=1)
        frm_left_side.rowconfigure(2, weight=1)

        frm_smyt = tk.Frame(frm_left_side, relief="sunken", borderwidth=2)
        frm_smyt.grid(row=0, column=0, sticky="new")

        tk.Label(frm_smyt, text="SMYT", font="TkHeadingFont, 22", padx=10, pady=10).pack(expand=True)

        self._var_status = tk.StringVar(frm_left_side, "unknown")

        frm_status = tk.LabelFrame(frm_left_side, text="Status")
        frm_status.grid(row=1, column=0, pady=10, sticky="nsew")

        tk.Label(frm_status, textvariable=self._var_status).pack(expand=True)

        frm_buttons = tk.Frame(frm_left_side)
        frm_buttons.grid(row=2, column=0, sticky="sew")

        frm_buttons.columnconfigure(0, weight=1)
        frm_buttons.rowconfigure(0, weight=1)
        frm_buttons.rowconfigure(1, weight=1)
        frm_buttons.rowconfigure(2, weight=1)

        tk.Button(frm_buttons, text="Enable", command=None).grid(row=0, column=0, padx=5, sticky="ew")
        tk.Button(frm_buttons, text="Disable", command=None).grid(row=1, column=0, padx=5, pady=10, sticky="ew")
        tk.Button(frm_buttons, text="Help", command=self._on_help_button_pressed).grid(row=2, column=0, padx=5, sticky="ew")

    def _configure_contents(self):
        nbk_contents = ttk.Notebook(self)
        nbk_contents.grid(row=0, column=1, sticky="nsew")

        frm_page_logs = tk.Frame(nbk_contents, padx=25, pady=25)

        frm_page_logs.columnconfigure(0, weight=1)
        frm_page_logs.rowconfigure(0, weight=0)
        frm_page_logs.rowconfigure(1, weight=1)

        frm_buttons = tk.Frame(frm_page_logs)
        frm_buttons.grid(row=0, column=0, sticky="ne")

        frm_buttons.rowconfigure(0, weight=1)
        frm_buttons.columnconfigure(0, weight=1)
        frm_buttons.columnconfigure(1, weight=1)

        tk.Button(frm_buttons, text="Clear", command=self._on_clear_button_pressed).grid(row=0, column=0, padx=5, pady=5)
        tk.Button(frm_buttons, text="Refresh", command=None).grid(row=0, column=1, padx=5, pady=5)

        frm_logs = tk.Frame(frm_page_logs, padx=10, pady=10)
        frm_logs.grid(row=1, column=0, sticky="nsew")

        bar_logs = tk.Scrollbar(frm_logs, orient="vertical")
        bar_logs.pack(side="right", fill="y")

        self._lst_logs = tk.Listbox(frm_logs, yscrollcommand=bar_logs.set)
        self._lst_logs.pack(side="left", fill="both", expand=True)

        bar_logs.configure(command=self._lst_logs.yview)

        frm_page_configuration = tk.Frame(nbk_contents, padx=25, pady=25)

        tk.Label(frm_page_configuration, text="configuration").pack()

        nbk_contents.add(frm_page_logs, text="Logs")
        nbk_contents.add(frm_page_configuration, text="Configuration")

    def _on_help_button_pressed(self):
        pass

    def _on_clear_button_pressed(self):
        self._close_log_file()

        p1 = subprocess.Popen(["echo", "-n"], stdout=subprocess.PIPE)
        p2 = subprocess.Popen(["pkexec", "tee", self.LOG_FILE_PATH], stdin=p1.stdout)
        p1.stdout.close()
        p2.communicate()

        if p2.returncode != 0:
            print("Could not clear log file")
            return

        self._lst_logs.delete(0, "end")

    def _open_log_file(self) -> bool:
        self._logs = open(self.LOG_FILE_PATH, "r")

    def _close_log_file(self):
        if not self._logs:
            return

        if self._logs.closed:
            return

        self._logs.close()
        self._logs = None

    def _read_contents_log_file(self):
        try:
            self._open_log_file()
        except FileNotFoundError as err:
            print(f"Could not find log file: {err}")
            return
        except OSError as err:
            print(f"Could not open log file for reading: {err}")
            return

        lines = self._logs.readlines()

        for line in lines:
            self._lst_logs.insert("end", line.rstrip())

    def _on_window_closed(self):
        self._close_log_file()

        self.destroy()
        self._root.destroy()


def main() -> int:
    root = tk.Tk()
    Smyt(root)
    root.mainloop()

    return 0
