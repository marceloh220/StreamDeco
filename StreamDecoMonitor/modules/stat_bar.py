from __future__ import annotations
import customtkinter as ctk
from typing import Callable


class StatBar(ctk.CTkFrame):
    """
    A custom widget that displays a label and a progress bar to represent a hardware metric.
    The StatBar consists of a label that shows the name of the metric and its current value 
    (optionally formatted with a unit),
    and a progress bar that visually represents the value as a percentage of a maximum value.
    Attributes:
    - unit (str): The unit of measurement for the metric (e.g., "%", "°C"). 
        This is used to format the label text.
    - value_formatter (Callable[[float, float], str] | None): 
        An optional function to format the label text based on the current value and maximum value.
    """

    def __init__(self, master, label_text: str, unit: str = "", value_formatter: Callable[[float, float], str] | None = None, **kwargs):
        """
        Initializes the StatBar widget.
        Args:
            master: The parent widget.
            label_text (str): The text to display on the label.
            unit (str): The unit of measurement for the metric (e.g., "%", "°C").
            value_formatter (Callable[[float, float], str] | None):
                An optional function to format the label text based on the current value and maximum value.
        """
        super().__init__(master, fg_color="transparent", **kwargs)
        self.unit = unit
        self.value_formatter = value_formatter
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
        """
        Updates the progress bar and label based on the current value and maximum value.
        Args:
            value (float): The current value of the metric to display.
            max_value (float): The maximum value for the metric,
                used to calculate the percentage for the progress bar.
        """
        max_value = max(1.0, max_value)
        normalized = max(0.0, min(1.0, value / max_value))
        self.progress.set(normalized)
        if self.value_formatter is not None:
            self.label.configure(text=self.value_formatter(value, max_value))
            return
        if self.unit:
            self.label.configure(text=f"{int(value)} {self.unit}")
        else:
            self.label.configure(text=f"{int(value)}")
