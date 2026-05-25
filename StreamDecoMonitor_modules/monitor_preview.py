from __future__ import annotations
from datetime import datetime
from typing import Callable
import customtkinter as ctk
from queue import Empty, Queue
from .circular_gauge import CircularGauge
from .stat_bar import StatBar

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class MonitorPreview(ctk.CTk):

    def __init__(self, queue_metrics: Queue[dict[str, float]], on_exit: Callable[[], None]) -> None:
        super().__init__()
        self._queue_metrics = queue_metrics
        self._on_exit_function: Callable = on_exit
        self._metrics_update_ms = 1200
        self.title("StreamDeco Monitor - CustomTkinter")
        self.geometry("1024x600")
        self.minsize(960, 560)
        self.view_mode = "monitor"
        self.metrics_paused = False
        self.protocol("WM_DELETE_WINDOW", self._on_exit)
        self._build_layout()
        self._build_cards()
        self._build_controls()
        self._apply_view_mode()
        self._update_metrics()
        self._update_clock()

    def _build_layout(self) -> None:
        self.outer = ctk.CTkFrame(self, fg_color="#f3a45f", corner_radius=0)
        self.outer.pack(fill="both", expand=True)
        self.content = ctk.CTkFrame(self.outer, fg_color="transparent")
        self.content.pack(fill="both", expand=True, padx=20, pady=20)
        self.content.grid_columnconfigure(0, weight=1)
        self.content.grid_columnconfigure(1, weight=0)
        self.main_panel = ctk.CTkFrame(
            self.content,
            fg_color="#2a414f",
            corner_radius=14,
            border_width=0,
        )
        self.main_panel.grid(row=0, column=0, sticky="nsew", padx=(0, 20))
        self.main_panel.grid_rowconfigure(0, weight=1)
        self.main_panel.grid_rowconfigure(1, weight=1)
        self.main_panel.grid_columnconfigure(0, weight=1)
        self.main_panel.grid_columnconfigure(1, weight=1)

    def _build_cards(self) -> None:
        self.center_separator = ctk.CTkFrame(self.main_panel, fg_color="#335061", corner_radius=0, height=2)
        self.center_separator.grid(row=1, column=0, columnspan=2, sticky="new")
        self._build_cpu_card()
        self._build_mem_card()
        self._build_gpu_card()
        self._build_clock_card()

    def _build_controls(self) -> None:
        self._build_side_buttons(self.content)

    def _set_status_mode(self, text: str) -> None:
        self.status_label.configure(text=text)

    def _create_card_header(self, parent: ctk.CTkFrame, text: str) -> ctk.CTkLabel:
        header = ctk.CTkLabel(
            parent,
            text=text,
            text_color="#e7effa",
            font=("Segoe UI", 34, "bold"),
        )
        return header

    def _create_metric_stats(self, parent: ctk.CTkFrame) -> ctk.CTkFrame:
        stats = ctk.CTkFrame(parent, fg_color="transparent")
        stats.grid_columnconfigure(0, weight=1)
        return stats

    def _create_side_button(self, parent: ctk.CTkFrame, **kwargs) -> ctk.CTkButton:
        button_style = {
            "width": 150,
            "height": 120,
            "corner_radius": 10,
            "fg_color": "#c8d4f4",
            "hover_color": "#b7c7f2",
            "text_color": "#1f2a40",
            "font": ("Segoe UI", 38, "bold"),
        }
        button_style.update(kwargs)
        return ctk.CTkButton(parent, **button_style)

    def _build_metric_card(self, parent: ctk.CTkFrame, title: str, gauge_size: int = 110) -> tuple[ctk.CTkFrame, CircularGauge, ctk.CTkFrame]:
        card = ctk.CTkFrame(parent, fg_color="transparent")
        card.grid_columnconfigure(1, weight=1)
        header = self._create_card_header(card, title)
        header.grid(row=0, column=0, columnspan=2, sticky="w", pady=(0, 10))
        gauge = CircularGauge(card, size=gauge_size)
        gauge.grid(row=1, column=0, sticky="w", padx=(0, 16))
        stats = self._create_metric_stats(card)
        stats.grid(row=1, column=1, sticky="ew")
        return card, gauge, stats

    def _build_memory_card(self, parent: ctk.CTkFrame) -> ctk.CTkFrame:
        card = ctk.CTkFrame(parent, fg_color="transparent")
        card.grid_columnconfigure(0, weight=1)
        header = self._create_card_header(card, "MEM")
        header.grid(row=0, column=0, sticky="w", pady=(0, 14))
        self.mem_ram = StatBar(card, "RAM: 0 MB")
        self.mem_ram.grid(row=1, column=0, sticky="ew", pady=(0, 12))
        self.mem_disk = StatBar(card, "C: 0/0 GB")
        self.mem_disk.grid(row=2, column=0, sticky="ew")
        return card

    def _format_ram_text(self, used_mb: float, total_mb: float) -> str:
        return f"RAM: {int(used_mb)}/{int(total_mb)} MB"

    def _format_disk_text(self, used_gb: float, total_gb: float) -> str:
        return f"C: {int(used_gb)}/{int(total_gb)} GB"

    def _apply_measurements(self, data: dict[str, float]) -> None:
        self.cpu_gauge.set_value(data["cpu_load"])
        self.gpu_gauge.set_value(data["gpu_load"])
        self.cpu_temp.set_value(data["cpu_temp"], 120)
        self.cpu_freq.set_value(data["cpu_freq"], 6000)
        self.gpu_temp.set_value(data["gpu_temp"], 120)
        self.gpu_freq.set_value(data["gpu_freq"], 4000)
        self.mem_ram.label.configure(text=self._format_ram_text(data["ram_used"], data["ram_total"]))
        self.mem_ram.set_value(data["ram_used"], data["ram_total"])
        self.mem_disk.label.configure(text=self._format_disk_text(data["disk_used"], data["disk_total"]))
        self.mem_disk.set_value(data["disk_used"], data["disk_total"])

    def _build_cpu_card(self):
        self.cpu_card, self.cpu_gauge, stats = self._build_metric_card(self.main_panel, "CPU")
        self.cpu_card.grid(row=0, column=0, sticky="nsew", padx=24, pady=(18, 12))
        self.cpu_temp = StatBar(stats, "0 C", unit="C")
        self.cpu_temp.grid(row=0, column=0, sticky="ew", pady=(4, 12))
        self.cpu_freq = StatBar(stats, "0 MHz", unit="MHz")
        self.cpu_freq.grid(row=1, column=0, sticky="ew")

    def _build_mem_card(self):
        self.mem_card = self._build_memory_card(self.main_panel)
        self.mem_card.grid(row=0, column=1, sticky="nsew", padx=24, pady=(18, 12))

    def _build_gpu_card(self):
        self.gpu_card, self.gpu_gauge, stats = self._build_metric_card(self.main_panel, "GPU")
        self.gpu_card.grid(row=2, column=0, sticky="nsew", padx=24, pady=(14, 18))
        self.gpu_temp = StatBar(stats, "0 C", unit="C")
        self.gpu_temp.grid(row=0, column=0, sticky="ew", pady=(4, 12))
        self.gpu_freq = StatBar(stats, "0 MHz", unit="MHz")
        self.gpu_freq.grid(row=1, column=0, sticky="ew")

    def _build_clock_card(self):
        self.clock_card = ctk.CTkFrame(self.main_panel, fg_color="transparent")
        self.clock_card.grid(row=2, column=1, sticky="nsew", padx=24, pady=(14, 18))
        self.clock_content = ctk.CTkFrame(self.clock_card, fg_color="transparent")
        self.clock_content.place(relx=0.0, rely=0.0, anchor="nw", x=0, y=28)
        self.clock_label = ctk.CTkLabel(
            self.clock_content,
            text="00:00:00",
            text_color="#eaf1fb",
            font=("Segoe UI", 58, "bold"),
        )
        self.clock_label.pack(anchor="w", pady=(0, 8))
        self.date_label = ctk.CTkLabel(
            self.clock_content,
            text="01/01/2026",
            text_color="#d7e2ef",
            font=("Segoe UI", 32, "bold"),
        )
        self.date_label.pack(anchor="w")

    def _set_clock_centered(self, centered: bool) -> None:
        if centered:
            self.clock_content.place(relx=0.5, rely=0.5, anchor="center")
            self.clock_label.pack_configure(anchor="center", pady=(0, 8))
            self.date_label.pack_configure(anchor="center")
            self.clock_label.configure(justify="center")
            self.date_label.configure(justify="center")
        else:
            self.clock_content.place(relx=0.0, rely=0.0, anchor="nw", x=0, y=28)
            self.clock_label.pack_configure(anchor="w", pady=(0, 8))
            self.date_label.pack_configure(anchor="w")
            self.clock_label.configure(justify="left")
            self.date_label.configure(justify="left")

    def _build_side_buttons(self, parent):
        side = ctk.CTkFrame(parent, fg_color="transparent", width=170)
        side.grid(row=0, column=1, sticky="ns")
        self.btn_top = self._create_side_button(side, text=">", command=None)
        self.btn_top.pack(pady=(6, 14))
        self.btn_mid = self._create_side_button(side, text="|||", command=None)
        self.btn_mid.pack(pady=14)
        self.btn_bot = self._create_side_button(
            side,
            text="==",
            width=150,
            height=120,
            corner_radius=10,
            fg_color="#304d5e",
            hover_color="#3e6074",
            text_color="#edf5ff",
            font=("Segoe UI", 34, "bold"),
            command=None,
        )
        self.btn_bot.pack(pady=(14, 6))
        self.status_label = ctk.CTkLabel(
            side,
            text="MODE: MONITOR",
            text_color="#25313c",
            font=("Segoe UI", 14, "bold"),
        )
        self.status_label.pack(pady=(10, 0))

    def btn_top_command(self, function) -> None:
        self.btn_top.configure(command=function)

    def btn_mid_command(self, function) -> None:
        self.btn_mid.configure(command=function)

    def btn_bot_command(self, function) -> None:
        self.btn_bot.configure(command=function)

    def _apply_view_mode(self) -> None:
        self.cpu_card.grid_remove()
        self.mem_card.grid_remove()
        self.gpu_card.grid_remove()
        self.clock_card.grid_remove()
        self.center_separator.grid_remove()
        if self.view_mode == "monitor":
            self.cpu_card.grid(row=0, column=0, sticky="nsew", padx=24, pady=(18, 12))
            self.mem_card.grid(row=0, column=1, sticky="nsew", padx=24, pady=(18, 12))
            self.center_separator.grid(row=1, column=0, columnspan=2, sticky="new")
            self.gpu_card.grid(row=2, column=0, columnspan=1, sticky="nsew", padx=24, pady=(14, 18))
            self.clock_card.grid(row=2, column=1, sticky="nsew", padx=24, pady=(14, 18))
            self._set_clock_centered(False)
            self.gpu_temp.progress.configure(width=220)
            self.gpu_freq.progress.configure(width=220)
            self.update_idletasks()
            self._set_status_mode("MODE: MONITOR")
        elif self.view_mode == "performance":
            self.cpu_card.grid(row=0, column=0, sticky="nsew", padx=24, pady=(18, 12))
            self.mem_card.grid(row=0, column=1, sticky="nsew", padx=24, pady=(18, 12))
            self.center_separator.grid(row=1, column=0, columnspan=2, sticky="new")
            self.gpu_card.grid(row=2, column=0, columnspan=2, sticky="nsew", padx=24, pady=(14, 18))
            self._set_clock_centered(False)
            self._set_status_mode("MODE: PERFORMANCE")
        else:
            self.clock_card.grid(row=0, column=0, columnspan=2, rowspan=3, sticky="nsew", padx=28, pady=28)
            self._set_clock_centered(True)
            self._set_status_mode("MODE: CLOCK")

    def cycle_view_mode(self) -> None:
        order = ["monitor", "performance", "clock"]
        idx = order.index(self.view_mode)
        self.view_mode = order[(idx + 1) % len(order)]
        self._apply_view_mode()

    def toggle_metrics_pause(self) -> None:
        self.metrics_paused = not self.metrics_paused
        if self.metrics_paused:
            self.btn_mid.configure(text=">")
            self._set_status_mode("MODE: PAUSED")
        else:
            self.btn_mid.configure(text="|||")
            self._apply_view_mode()

    def go_home_mode(self) -> None:
        self.view_mode = "monitor"
        self._apply_view_mode()

    def _on_exit(self) -> None:
        if self._on_exit_function:
            self._on_exit_function()
        self.destroy()

    def _get_latest_metrics(self) -> dict[str, float] | None:
        latest_data: dict[str, float] | None = None
        while True:
            try:
                latest_data = self._queue_metrics.get_nowait()
            except Empty:
                return latest_data

    def _update_metrics(self):
        if self.metrics_paused:
            self.after(self._metrics_update_ms, self._update_metrics)
            return
        data = self._get_latest_metrics()
        if data is not None:
            self._apply_measurements(data)
        self.after(self._metrics_update_ms, self._update_metrics)

    def _update_clock(self):
        now = datetime.now()
        self.clock_label.configure(text=now.strftime("%H:%M:%S"))
        self.date_label.configure(text=now.strftime("%d/%m/%Y"))
        self.after(500, self._update_clock)
