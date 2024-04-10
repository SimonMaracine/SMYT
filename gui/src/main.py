import tkinter as tk
import tkinter.ttk as ttk


class Smyt(tk.Frame):
    def __init__(self, root: tk.Tk):
        super().__init__(root)

        self._root = root
        self._configure()

    def _configure(self):
        self.pack(fill="both", expand=True, padx=20, pady=20)
        self.columnconfigure(0, weight=0)
        self.columnconfigure(1, weight=2)
        self.rowconfigure(0, weight=1)

        self._root.title("SMYT")
        self._root.geometry("768x432")

        self._configure_left_side()
        self._configure_contents()

    def _configure_left_side(self):
        frm_left_side = tk.Frame(self, relief="solid", borderwidth=1, padx=25, pady=20)
        frm_left_side.grid(row=0, column=0, sticky="NSW")

        frm_left_side.columnconfigure(0, weight=1)
        frm_left_side.rowconfigure(0, weight=1)
        frm_left_side.rowconfigure(1, weight=1)
        frm_left_side.rowconfigure(2, weight=1)

        frm_smyt = tk.Frame(frm_left_side, relief="sunken", borderwidth=4)
        frm_smyt.grid(row=0, column=0, sticky="NEW")

        tk.Label(frm_smyt, text="SMYT", font="TkHeadingFont, 22", padx=10, pady=10).pack(expand=True)

        self._var_status = tk.StringVar(frm_left_side, "unknown")

        frm_status = tk.LabelFrame(frm_left_side, text="Status")
        frm_status.grid(row=1, column=0, pady=10, sticky="NSEW")

        tk.Label(frm_status, textvariable=self._var_status).pack(expand=True)

        frm_buttons = tk.Frame(frm_left_side)
        frm_buttons.grid(row=2, column=0, sticky="SEW")

        frm_buttons.columnconfigure(0, weight=1)
        frm_buttons.rowconfigure(0, weight=1)
        frm_buttons.rowconfigure(1, weight=1)
        frm_buttons.rowconfigure(2, weight=1)

        tk.Button(frm_buttons, text="Enable", command=None).grid(row=0, column=0, padx=5, sticky="EW")
        tk.Button(frm_buttons, text="Disable", command=None).grid(row=1, column=0, padx=5, pady=10, sticky="EW")
        tk.Button(frm_buttons, text="Help", command=None).grid(row=2, column=0, padx=5, sticky="EW")

    def _configure_contents(self):
        nbk_contents = ttk.Notebook(self)
        nbk_contents.grid(row=0, column=1, sticky="NSEW")

        frm_page_logs = tk.Frame(nbk_contents, padx=25, pady=25)

        frm_page_logs.columnconfigure(0, weight=1)
        frm_page_logs.rowconfigure(0, weight=0)
        frm_page_logs.rowconfigure(1, weight=1)

        frm_buttons = tk.Frame(frm_page_logs)
        frm_buttons.grid(row=0, column=0, sticky="NE")

        frm_buttons.rowconfigure(0, weight=1)
        frm_buttons.columnconfigure(0, weight=1)
        frm_buttons.columnconfigure(1, weight=1)

        tk.Button(frm_buttons, text="Clear", command=None).grid(row=0, column=0, padx=5, pady=5)
        tk.Button(frm_buttons, text="Refresh", command=None).grid(row=0, column=1, padx=5, pady=5)

        frm_logs = tk.Frame(frm_page_logs, relief="sunken", borderwidth=1, padx=10, pady=10)
        frm_logs.grid(row=1, column=0, sticky="NSEW")

        # TODO temp
        tk.Label(frm_logs, text="Warning! Some log here. Bye.").pack()
        tk.Label(frm_logs, text="Warning! Some log there. Bye.").pack()
        tk.Label(frm_logs, text="Alert! Some log everywhere. Bye.").pack()

        frm_page_configuration = tk.Frame(nbk_contents, padx=25, pady=25)

        tk.Label(frm_page_configuration, text="configuration").pack()

        nbk_contents.add(frm_page_logs, text="Logs")
        nbk_contents.add(frm_page_configuration, text="Configuration")


def main() -> int:
    root = tk.Tk()
    Smyt(root)
    root.mainloop()

    return 0
