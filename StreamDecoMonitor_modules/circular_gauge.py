from __future__ import annotations
import tkinter as tk
import customtkinter as ctk


class CircularGauge(ctk.CTkFrame):

    def __init__(self, master, size: int = 92, thickness: int = 12, **kwargs):
        super().__init__(master, fg_color="transparent", **kwargs)
        self._size = size
        self._thickness = thickness
        self._value = 0
        self.canvas = tk.Canvas(
            self,
            width=size,
            height=size,
            bg="#2a414f",
            highlightthickness=0,
            bd=0,
        )
        self.canvas.pack()
        pad = thickness // 2 + 2
        self._bbox = (pad, pad, size - pad, size - pad)
        self.canvas.create_arc(
            self._bbox,
            start=90,
            extent=359.9,
            style=tk.ARC,
            width=thickness,
            outline="#496171",
        )
        self._arc_id = self.canvas.create_arc(
            self._bbox,
            start=90,
            extent=0,
            style=tk.ARC,
            width=thickness,
            outline="#e2ecff",
        )
        self._text_id = self.canvas.create_text(
            size // 2,
            size // 2,
            text="0%",
            fill="#e6eef8",
            font=("Segoe UI", 18, "bold"),
        )

    def set_value(self, value: float) -> None:
        self._value = max(0.0, min(100.0, value))
        extent = -(359.9 * (self._value / 100.0))
        self.canvas.itemconfigure(self._arc_id, extent=extent)
        self.canvas.itemconfigure(self._text_id, text=f"{int(self._value)}%")
