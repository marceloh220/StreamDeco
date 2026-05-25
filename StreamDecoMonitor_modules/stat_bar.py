from __future__ import annotations
import customtkinter as ctk


class StatBar(ctk.CTkFrame):

    def __init__(self, master, label_text: str, unit: str = "", **kwargs):
        super().__init__(master, fg_color="transparent", **kwargs)
        self.unit = unit
        self.grid_columnconfigure(0, weight=1)
        self.label = ctk.CTkLabel(
            self,
            text=label_text,
            text_color="#dfe9f5",
            font=("Segoe UI", 17, "bold"),
            anchor="w",
        )
        self.label.grid(row=0, column=0, sticky="w", pady=(0, 6))
        self.progress = ctk.CTkProgressBar(
            self,
            width=220,
            height=15,
            corner_radius=8,
            fg_color="#495f70",
            progress_color="#f0f3fb",
        )
        self.progress.grid(row=1, column=0, sticky="ew")
        self.progress.set(0)

    def set_value(self, value: float, max_value: float = 100.0) -> None:
        max_value = max(1.0, max_value)
        normalized = max(0.0, min(1.0, value / max_value))
        self.progress.set(normalized)
        if self.unit:
            self.label.configure(text=f"{int(value)} {self.unit}")
        else:
            self.label.configure(text=f"{int(value)}")
