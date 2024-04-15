import subprocess
import io
import tkinter as tk
import tkinter.ttk as ttk
from tkinter import messagebox

import task
import configuration

# https://tkdocs.com/tutorial/index.html
# https://linux.die.net/man/1/pkexec


class LogError(Exception):
    pass


class Smyt(tk.Frame):
    LOG_FILE_PATH = "/var/log/smyt/smyt.log"
    STATUS_UNKNOWN = "unknown"
    STATUS_ACTIVE = "active"
    STATUS_INACTIVE = "inactive"
    STATUS_UNINSTALLED = "uninstalled"
    BLACK = "black"
    GREEN = "green"
    RED = "dark red"
    VERSION = "0.1.0"

    def __init__(self, root: tk.Tk):
        super().__init__(root)

        self._root = root

        self._logs = None
        self._logs_reader = task.Task(self, self._read_log_file, 10_000)

        self._config: configuration.Config = None
        self._config_saver = task.Task(self, self._tick_configuration_saver, 1000)
        self._config_timer: int = None

        self._service_watcher = task.Task(self, self._get_service_status, 25_000)

        self._configure()

        try:
            self._open_log_file()
        except LogError as err:
            messagebox.showerror("Log Error", err)
        else:
            self._read_log_file()
            self._logs_reader.start()

        self._get_service_status()
        self._service_watcher.start()

        self._read_configuration_file()

    def _configure(self):
        self.pack(fill="both", expand=True, padx=20, pady=20)
        self.columnconfigure(0, weight=0)
        self.columnconfigure(1, weight=2)
        self.rowconfigure(0, weight=1)

        self._root.title("SMYT GUI")
        self._root.geometry("768x432")
        self._root.protocol("WM_DELETE_WINDOW", self._on_window_closed)
        self._root.minsize(512, 288)

        self._configure_left_side()
        self._configure_contents()

    def _configure_left_side(self):
        frm_left_side = tk.Frame(self)
        frm_left_side.grid(row=0, column=0, sticky="nsw", padx=(0, 20), pady=20)

        frm_left_side.columnconfigure(0, weight=1)
        frm_left_side.rowconfigure(0, weight=2)
        frm_left_side.rowconfigure(1, weight=1)
        frm_left_side.rowconfigure(2, weight=2)

        frm_smyt = tk.Frame(frm_left_side, relief="sunken", borderwidth=1)
        frm_smyt.grid(row=0, column=0, sticky="new")

        tk.Label(frm_smyt, text="SMYT", font="TkHeadingFont, 24", fg="gray20", padx=15, pady=10).pack(expand=True)

        self._var_status = tk.StringVar(frm_left_side, self.STATUS_UNKNOWN)

        frm_status = tk.LabelFrame(frm_left_side, text="Status")
        frm_status.grid(row=1, column=0, pady=10, sticky="nsew")

        self._lbl_status = tk.Label(frm_status, textvariable=self._var_status, font="TkHeadingFont, 12", fg=self.BLACK)
        self._lbl_status.pack(expand=True)

        frm_buttons = tk.Frame(frm_left_side)
        frm_buttons.grid(row=2, column=0, sticky="sew")

        frm_buttons.columnconfigure(0, weight=1)
        frm_buttons.rowconfigure(0, weight=1)
        frm_buttons.rowconfigure(1, weight=1)
        frm_buttons.rowconfigure(2, weight=1)

        tk.Button(frm_buttons, text="Start", command=self._on_start_button_pressed).grid(row=0, column=0, padx=5, sticky="ew")
        tk.Button(frm_buttons, text="Stop", command=self._on_stop_button_pressed).grid(row=1, column=0, padx=5, pady=10, sticky="ew")
        tk.Button(frm_buttons, text="Help", command=self._on_help_button_pressed).grid(row=2, column=0, padx=5, sticky="ew")

    def _configure_contents(self):
        nbk_contents = ttk.Notebook(self)
        nbk_contents.grid(row=0, column=1, sticky="nsew")

        frm_page_logs = self._configure_logs(nbk_contents)
        frm_page_configuration = self._configure_configuration(nbk_contents)

        nbk_contents.add(frm_page_logs, text="Logs")
        nbk_contents.add(frm_page_configuration, text="Configuration")

    def _configure_logs(self, nbk_contents):
        frm_page_logs = tk.Frame(nbk_contents, padx=20, pady=20)

        frm_page_logs.columnconfigure(0, weight=1)
        frm_page_logs.rowconfigure(0, weight=0)
        frm_page_logs.rowconfigure(1, weight=1)

        frm_buttons = tk.Frame(frm_page_logs)
        frm_buttons.grid(row=0, column=0, sticky="ne")

        tk.Button(frm_buttons, text="Clear", command=self._on_clear_button_pressed).pack(expand=True, pady=10)

        frm_logs = tk.Frame(frm_page_logs)
        frm_logs.grid(row=1, column=0, sticky="nsew")

        bar_logs = tk.Scrollbar(frm_logs, orient="vertical")
        bar_logs.pack(side="right", fill="y")

        self._lst_logs = tk.Listbox(frm_logs, yscrollcommand=bar_logs.set)
        self._lst_logs.pack(side="left", fill="both", expand=True)

        bar_logs.configure(command=self._lst_logs.yview)

        return frm_page_logs

    def _configure_configuration(self, nbk_contents):
        frm_page_configuration = tk.Frame(nbk_contents, padx=20, pady=20)

        frm_page_configuration.columnconfigure(0, weight=1)
        frm_page_configuration.rowconfigure(0, weight=0)
        frm_page_configuration.rowconfigure(1, weight=1)

        frm_buttons = tk.Frame(frm_page_configuration)
        frm_buttons.grid(row=0, column=0, sticky="nw", pady=(0, 20))

        frm_buttons.rowconfigure(0, weight=1)
        frm_buttons.columnconfigure(0, weight=1)
        frm_buttons.columnconfigure(1, weight=1)
        frm_buttons.columnconfigure(2, weight=1)
        frm_buttons.columnconfigure(3, weight=1)

        self._btn_edit = tk.Button(frm_buttons, text="Edit", command=self._on_edit_button_pressed, state="active")
        self._btn_edit.grid(row=0, column=0, pady=(0, 10))
        self._btn_discard = tk.Button(frm_buttons, text="Discard", command=self._on_discard_button_pressed, state="disabled")
        self._btn_discard.grid(row=0, column=1, padx=10, pady=(0, 10))
        self._btn_save = tk.Button(frm_buttons, text="Save", command=self._on_save_button_pressed, state="disabled")
        self._btn_save.grid(row=0, column=2, pady=(0, 10))

        self._var_config_timer = tk.StringVar(frm_buttons, value="")

        tk.Label(frm_buttons, textvariable=self._var_config_timer).grid(row=0, column=3, padx=(20, 0), pady=(0, 10))

        frm_options = tk.Frame(frm_page_configuration)
        frm_options.grid(row=1, column=0, sticky="nsew")

        frm_options.columnconfigure(0, weight=1)
        frm_options.columnconfigure(1, weight=3)
        frm_options.rowconfigure(0, weight=1)
        frm_options.rowconfigure(1, weight=1)
        frm_options.rowconfigure(2, weight=1)
        frm_options.rowconfigure(3, weight=4)

        self._var_process_period = tk.StringVar(frm_options)
        self._var_warning_threshold = tk.StringVar(frm_options)
        self._var_panic_threshold = tk.StringVar(frm_options)
        self._var_device = tk.StringVar(frm_options)

        tk.Label(frm_options, text="process_period").grid(row=0, column=0, sticky="new")
        tk.Label(frm_options, text="warning_threshold").grid(row=1, column=0, sticky="new")
        tk.Label(frm_options, text="panic_threshold").grid(row=2, column=0, sticky="new")
        tk.Label(frm_options, text="device").grid(row=3, column=0, sticky="new")

        self._ent_process_period = tk.Entry(frm_options, textvariable=self._var_process_period, state="disabled")
        self._ent_process_period.grid(row=0, column=1, sticky="new", padx=10, pady=(0, 10))
        self._ent_warning_threshold = tk.Entry(frm_options, textvariable=self._var_warning_threshold, state="disabled")
        self._ent_warning_threshold.grid(row=1, column=1, sticky="new", padx=10, pady=(0, 10))
        self._ent_panic_threshold = tk.Entry(frm_options, textvariable=self._var_panic_threshold, state="disabled")
        self._ent_panic_threshold.grid(row=2, column=1, sticky="new", padx=10, pady=(0, 10))
        self._ent_device = tk.Entry(frm_options, textvariable=self._var_device, state="disabled")
        self._ent_device.grid(row=3, column=1, sticky="new", padx=10, pady=(0, 10))

        return frm_page_configuration

    def _on_start_button_pressed(self):
        result = self._start_service()

        if result != 0:
            messagebox.showerror("Service Start Failure", f"Failed to start the service. Error code: {result}")
            return

        self._var_status.set(self.STATUS_ACTIVE)
        self._lbl_status["fg"] = self.GREEN

    def _on_stop_button_pressed(self):
        result = self._stop_service()

        if result != 0:
            messagebox.showerror("Service Stop Failure", f"Failed to stop the service. Error code: {result}")
            return

        self._var_status.set(self.STATUS_INACTIVE)
        self._lbl_status["fg"] = self.RED

    def _on_help_button_pressed(self):
        messagebox.showinfo("Help", f"SMYT GUI version {self.VERSION}")

    def _on_clear_button_pressed(self):
        self._logs_reader.stop()
        self._close_log_file()

        result = self._truncate_log_file()

        try:
            self._open_log_file()
        except LogError as err:
            messagebox.showerror("Log Error", err)
            return

        self._logs.seek(0, io.SEEK_END)  # FIXME logs printed while file was closed are missed
        self._logs_reader.start()

        if result == 0:
            self._lst_logs.delete(0, "end")
        else:
            messagebox.showerror("Log Error", f"Could not clear log file. Error code: {result}")

    def _on_edit_button_pressed(self):
        self._config_timer = 30
        self._var_config_timer.set(self._config_timer)
        self._config_saver.start()

        self._ent_process_period["state"] = "normal"
        self._ent_warning_threshold["state"] = "normal"
        self._ent_panic_threshold["state"] = "normal"
        self._ent_device["state"] = "normal"

        self._btn_edit["state"] = "disabled"
        self._btn_discard["state"] = "active"
        self._btn_save["state"] = "active"

    def _on_discard_button_pressed(self):
        self._discard_configuration_changes()

    def _on_save_button_pressed(self):
        self._config_saver.stop()
        self._var_config_timer.set("")
        self._config_timer = None

        self._ent_process_period["state"] = "disabled"
        self._ent_warning_threshold["state"] = "disabled"
        self._ent_panic_threshold["state"] = "disabled"
        self._ent_device["state"] = "disabled"

        self._btn_edit["state"] = "active"
        self._btn_discard["state"] = "disabled"
        self._btn_save["state"] = "disabled"

        config = configuration.Config(
            self._var_process_period.get(),
            self._var_warning_threshold.get(),
            self._var_panic_threshold.get(),
            self._var_device.get()
        )

        contents = configuration.dump_configuration(config)
        result = self._write_configuration_file(contents)

        if result != 0:
            self._discard_configuration_changes()
            messagebox.showerror("Configuration Error", "Could not write to configuration.")
            return

        self._config = config

    def _open_log_file(self):
        assert self._logs is None

        try:
            self._logs = open(self.LOG_FILE_PATH, "r")
        except FileNotFoundError as err:
            raise LogError(f"Could not find log file: {err}")
        except OSError as err:
            raise LogError(f"Could not open log file for reading: {err}")

    def _close_log_file(self):
        if not self._logs:
            return

        if self._logs.closed:
            return

        self._logs.close()
        self._logs = None

    def _truncate_log_file(self) -> int:
        p1 = subprocess.Popen(["echo", "-n"], stdout=subprocess.PIPE)
        p2 = subprocess.Popen(["pkexec", "tee", self.LOG_FILE_PATH], stdin=p1.stdout)
        p1.stdout.close()
        p2.communicate()

        return p2.returncode

    def _check_service_status(self) -> int:
        p = subprocess.run(["systemctl", "is-active", "--quiet", "smyt.service"])

        return p.returncode

    def _start_service(self) -> int:
        p = subprocess.run(["pkexec", "systemctl", "start", "smyt.service"])

        return p.returncode

    def _stop_service(self) -> int:
        p = subprocess.run(["pkexec", "systemctl", "stop", "smyt.service"])

        return p.returncode

    def _write_configuration_file(self, contents: str) -> int:
        p1 = subprocess.Popen(["echo", contents], stdout=subprocess.PIPE)
        p2 = subprocess.Popen(["pkexec", "tee", configuration.CONFIGURATION_FILE_PATH], stdin=p1.stdout)
        p1.stdout.close()
        p2.communicate()

        return p2.returncode

    def _read_configuration_file(self):
        try:
            config = configuration.read_configuration()
        except configuration.ConfigError as err:
            messagebox.showerror("Configuration Error", err)
            config = configuration.Config()

        self._var_process_period.set(config.process_period)
        self._var_warning_threshold.set(config.warning_threshold)
        self._var_panic_threshold.set(config.panic_threshold)
        self._var_device.set(config.device)

        self._config = config

    def _read_log_file(self) -> bool:
        if not self._logs:
            return

        lines = self._logs.readlines()

        for line in lines:
            self._lst_logs.insert("end", line.rstrip())

        return True

    def _tick_configuration_saver(self) -> bool:
        assert self._config_timer is not None

        self._config_timer -= 1
        self._var_config_timer.set(self._config_timer)

        if self._config_timer <= 0:
            self._discard_configuration_changes()
            return False

        return True

    def _get_service_status(self) -> bool:
        result = self._check_service_status()

        match result:
            case 0:
                self._var_status.set(self.STATUS_ACTIVE)
                self._lbl_status["fg"] = self.GREEN
            case 3:
                self._var_status.set(self.STATUS_INACTIVE)
                self._lbl_status["fg"] = self.RED
            case 4:
                self._var_status.set(self.STATUS_UNINSTALLED)
                self._lbl_status["fg"] = self.RED
            case _:
                self._var_status.set(self.STATUS_UNKNOWN)
                self._lbl_status["fg"] = self.BLACK

        return True

    def _discard_configuration_changes(self):
        self._var_process_period.set(self._config.process_period)
        self._var_warning_threshold.set(self._config.warning_threshold)
        self._var_panic_threshold.set(self._config.panic_threshold)
        self._var_device.set(self._config.device)

        self._config_saver.stop()
        self._var_config_timer.set("")
        self._config_timer = None

        self._ent_process_period["state"] = "disabled"
        self._ent_warning_threshold["state"] = "disabled"
        self._ent_panic_threshold["state"] = "disabled"
        self._ent_device["state"] = "disabled"

        self._btn_edit["state"] = "active"
        self._btn_discard["state"] = "disabled"
        self._btn_save["state"] = "disabled"

    def _on_window_closed(self):
        self._close_log_file()

        self.destroy()
        self._root.destroy()


def main() -> int:
    root = tk.Tk()
    Smyt(root)
    root.mainloop()

    return 0
