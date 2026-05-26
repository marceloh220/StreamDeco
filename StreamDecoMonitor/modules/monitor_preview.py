from __future__ import annotations
from datetime import datetime
from pathlib import Path
from typing import Callable
import sys
import customtkinter as ctk
from queue import Empty, Queue
from PIL import Image
from .circular_gauge import CircularGauge
from .stat_bar import StatBar

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")


class MonitorPreview(ctk.CTk):
    """
    A custom Tkinter window for monitoring hardware metrics.
    This class provides a graphical interface for displaying CPU, GPU, and memory usage,
    as well as a clock and other system information.
    It includes methods for building the layout, updating metrics, and handling user interactions.
    The window is designed to be visually appealing and easy to read, with a focus on providing
    real-time hardware monitoring information in a format inspired by the StreamDeco monitor layout.
    """

    _ICON_SIZE = (22, 22)

    def __init__(self, queue_metrics: Queue[dict[str, float]],
                 on_exit: Callable[[], None], update_interval_milliseconds: float = 500.0) -> None:
        """
        Initializes the MonitorPreview window.
        Args:
            queue_metrics (Queue[dict[str, float]]): A queue for receiving hardware metrics data
                from a background thread or process. The data should be in the form of a dictionary
                containing metric names and their corresponding float values.
            on_exit (Callable[[], None]): A callback function to be called when the window is closed.
                This allows for proper cleanup of resources and stopping of background tasks when the
                user exits the application.
            update_interval_milliseconds (float): The interval in milliseconds for updating the displayed metrics.
        """
        super().__init__()
        self._queue_metrics = queue_metrics
        self._on_exit_function: Callable = on_exit
        self._metrics_update_ms = int(update_interval_milliseconds)
        self.title("StreamDeco Monitor - CustomTkinter")
        self.geometry("1024x600")
        self.minsize(960, 560)
        self.view_mode = "monitor"
        self.metrics_paused = False
        self._header_icons: dict[str, ctk.CTkImage] = {}
        self.protocol("WM_DELETE_WINDOW", self._on_exit)
        self._build_layout()
        self._build_cards()
        self._build_controls()
        self._apply_view_mode()
        self._update_metrics()
        self._update_clock()

    def _build_layout(self) -> None:
        """
        Builds the main layout of the MonitorPreview window, including the outer frame,
        content frame, and main panel. This method sets up the grid configuration and
        prepares the window for adding cards and controls.
        """
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
        """
        Builds the hardware metric cards for CPU, memory, GPU, and clock.
        This method creates the individual cards and arranges them within the main panel.
        """
        self.center_separator = ctk.CTkFrame(self.main_panel, fg_color="#335061", corner_radius=0, height=2)
        self.center_separator.grid(row=1, column=0, columnspan=2, sticky="new")
        self._build_cpu_card()
        self._build_memory_card()
        self._build_gpu_card()
        self._build_clock_card()

    def _build_controls(self) -> None:
        """
        Builds the control buttons for the MonitorPreview window.
        This method creates the side buttons and arranges them within the content frame.
        """
        self._build_side_buttons(self.content)

    def _set_status_mode(self, text: str) -> None:
        """
        Updates the status label text to reflect the current mode of the monitor.
        Args:            
            text (str): The text to display in the status label, indicating the current mode 
            (e.g., "MODE: MONITOR", "MODE: PERFORMANCE", "MODE: CLOCK", "MODE: PAUSED").
        """
        self.status_label.configure(text=text)

    def _assets_dir(self) -> Path:
        """
        Determines the path to the assets directory, which contains icons and other resources.
        This method checks if the application is running in a frozen state (e.g., as a bundled executable) 
        and returns the appropriate path to the assets directory.
        Returns:
            Path: The path to the assets directory.
        """
        if getattr(sys, "frozen", False):
            return Path(getattr(sys, "_MEIPASS")) / "assets"
        return Path(__file__).resolve().parents[1] / "assets"

    def _load_header_icon(self, icon_name: str) -> ctk.CTkImage:
        """
        Loads and processes an icon image for use in the card headers.
        This method checks if the requested icon is already cached and returns it if available.
        If the icon is not cached, it loads the image from the assets directory, converts it to RGBA,
        creates a white version of the icon using the alpha channel, and then creates a CTkImage
        that can be used in the card headers. The processed icon is then cached for future use.
        Args:
            icon_name (str): The name of the icon file to load.
        Returns:
            ctk.CTkImage: The processed CTkImage object for the icon.
        """
        cached = self._header_icons.get(icon_name)
        if cached is not None:
            return cached

        icon_path = self._assets_dir() / icon_name
        image = Image.open(icon_path).convert("RGBA")
        alpha = image.getchannel("A")
        white = Image.new("RGBA", image.size, (255, 255, 255, 255))
        white.putalpha(alpha)
        icon = ctk.CTkImage(light_image=white, dark_image=white, size=self._ICON_SIZE)
        self._header_icons[icon_name] = icon
        return icon

    def _create_card_header(self, parent: ctk.CTkFrame, text: str, icon_name: str) -> ctk.CTkFrame:
        """
        Creates a header for a metric card, consisting of an icon and a text label.
        This method creates a frame for the header, loads the specified icon, and creates a label.
        Args:
            parent (ctk.CTkFrame): The parent frame for the header.
            text (str): The text to display in the header.
            icon_name (str): The name of the icon file to load.
        Returns:
            ctk.CTkFrame: The created header frame.
        """
        header = ctk.CTkFrame(parent, fg_color="transparent")
        header.grid_columnconfigure(1, weight=1)
        icon = ctk.CTkLabel(header, text="", image=self._load_header_icon(icon_name))
        icon.grid(row=0, column=0, sticky="w", padx=(0, 10))
        label = ctk.CTkLabel(
            header,
            text=text,
            text_color="#e7effa",
            font=("Segoe UI", 32, "bold"),
        )
        label.grid(row=0, column=1, sticky="w")
        return header

    def _create_metric_stats(self, parent: ctk.CTkFrame) -> ctk.CTkFrame:
        """
        Creates a frame for displaying additional metric statistics below the main gauge.
        This method creates a transparent frame that can be populated with StatBar widgets or other
        controls to show detailed information about the metrics.
        Args:
            parent (ctk.CTkFrame): The parent frame for the stats.
        Returns:
            ctk.CTkFrame: The created stats frame.
        """
        stats = ctk.CTkFrame(parent, fg_color="transparent")
        stats.grid_columnconfigure(0, weight=1)
        return stats

    def _create_side_button(self, parent: ctk.CTkFrame, **kwargs) -> ctk.CTkButton:
        """
        Creates a side button with a consistent style.
        Args:
            parent (ctk.CTkFrame): The parent frame for the button.
            **kwargs: Additional keyword arguments to customize the button.
        Returns:
            ctk.CTkButton: The created side button.
        """
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

    def _create_gauge_metric(
            self, parent: ctk.CTkFrame, title: str, icon_name: str, gauge_size: int = 110
        ) -> tuple[ctk.CTkFrame, CircularGauge, ctk.CTkFrame]:
        """
        Creates a gauge metric with a header, a circular gauge, and a stats frame.
        This method creates a card for a hardware metric that includes a header with an icon and title,
        a circular gauge to display the main metric value, and a stats frame for additional information.
        Args:
            parent (ctk.CTkFrame): The parent frame for the gauge metric.
            title (str): The title of the gauge metric.
            icon_name (str): The name of the icon file to load for the header.
            gauge_size (int, optional): The size of the circular gauge. Defaults to 110.
        """
        card = ctk.CTkFrame(parent, fg_color="transparent")
        card.grid_columnconfigure(1, weight=1)
        header = self._create_card_header(card, title, icon_name)
        header.grid(row=0, column=0, columnspan=2, sticky="w", pady=(0, 12))
        gauge = CircularGauge(card, size=gauge_size)
        gauge.grid(row=1, column=0, sticky="w", padx=(0, 16))
        stats = self._create_metric_stats(card)
        stats.grid(row=1, column=1, sticky="ew")
        return card, gauge, stats

    def _create_memory_metric(self, parent: ctk.CTkFrame) -> ctk.CTkFrame:
        """
        Creates a memory metric with a header and two stat bars for RAM and disk usage.
        Args:
            parent (ctk.CTkFrame): The parent frame for the memory card.
        Returns:
            ctk.CTkFrame: The created memory card frame.
        """
        card = ctk.CTkFrame(parent, fg_color="transparent")
        card.grid_columnconfigure(0, weight=1)
        header = self._create_card_header(card, "MEM", "ram_22_simp.png")
        header.grid(row=0, column=0, sticky="w", pady=(0, 12))
        self.mem_ram = StatBar(card, "RAM: 0/0 MB", value_formatter=self._format_ram_text)
        self.mem_ram.grid(row=1, column=0, sticky="ew", pady=(0, 12))
        self.mem_disk = StatBar(card, "C: 0/0 GB", value_formatter=self._format_disk_text)
        self.mem_disk.grid(row=2, column=0, sticky="ew")
        return card

    def _format_ram_text(self, used_mb: float, total_mb: float) -> str:
        """
        Formats the RAM usage text.
        Args:
            used_mb (float): The amount of used RAM in MB.
            total_mb (float): The total amount of RAM in MB.
        Returns:
            str: The formatted RAM usage text.
        """
        return f"RAM: {int(used_mb)}/{int(total_mb)} MB"

    def _format_disk_text(self, used_gb: float, total_gb: float) -> str:
        """
        Formats the disk usage text.
        Args:
            used_gb (float): The amount of used disk space in GB.
            total_gb (float): The total amount of disk space in GB.
        Returns:
            str: The formatted disk usage text.
        """
        return f"C: {int(used_gb)}/{int(total_gb)} GB"

    def _apply_measurements(self, data: dict[str, float]) -> None:
        """
        Applies the measurements from the data dictionary to the corresponding gauges and stat bars.
        Args:
            data (dict[str, float]): A dictionary containing the measurement data.
            The expected keys in the data dictionary include:
                - "cpu_load": The CPU load percentage.
                - "cpu_temp": The CPU temperature in degrees Celsius.
                - "cpu_freq": The CPU frequency in MHz.
                - "gpu_load": The GPU load percentage.
                - "gpu_temp": The GPU temperature in degrees Celsius.
                - "gpu_freq": The GPU frequency in MHz.
                - "ram_used": The amount of used RAM in MB.
                - "ram_total": The total amount of RAM in MB.
                - "disk_used": The amount of used disk space in GB.
                - "disk_total": The total amount of disk space in GB.
        This method updates the values of the CPU and GPU gauges, as well as the text and
        values of the RAM and disk stat bars based on the provided data.
        """
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
        """
        Builds the CPU metric card, which includes a circular gauge for 
        CPU load and stat bars for CPU temperature and frequency.
        """
        self.cpu_card, self.cpu_gauge, stats = self._create_gauge_metric(self.main_panel, "CPU", "processor_22_simp.png")
        self.cpu_card.grid(row=0, column=0, sticky="nsew", padx=24, pady=(18, 12))
        self.cpu_temp = StatBar(stats, "0 C", unit="C")
        self.cpu_temp.grid(row=0, column=0, sticky="ew", pady=(4, 12))
        self.cpu_freq = StatBar(stats, "0 MHz", unit="MHz")
        self.cpu_freq.grid(row=1, column=0, sticky="ew")

    def _build_memory_card(self):
        """
        Builds the memory metric card, which includes stat bars for RAM and disk usage.
        """
        self.mem_card = self._create_memory_metric(self.main_panel)
        self.mem_card.grid(row=0, column=1, sticky="nsew", padx=24, pady=(18, 12))

    def _build_gpu_card(self):
        """
        Builds the GPU metric card, which includes a circular gauge for 
        GPU load and stat bars for GPU temperature and frequency.
        """
        self.gpu_card, self.gpu_gauge, stats = self._create_gauge_metric(self.main_panel, "GPU", "gpu_22_simp.png")
        self.gpu_card.grid(row=2, column=0, sticky="nsew", padx=24, pady=(14, 18))
        self.gpu_temp = StatBar(stats, "0 C", unit="C")
        self.gpu_temp.grid(row=0, column=0, sticky="ew", pady=(4, 12))
        self.gpu_freq = StatBar(stats, "0 MHz", unit="MHz")
        self.gpu_freq.grid(row=1, column=0, sticky="ew")

    def _build_clock_card(self):
        """
        Builds the clock card, which includes a large label for the current time and a smaller label for the date.
        The clock is designed to be visually prominent and can be centered or left-aligned based on the view mode.
        """
        self.clock_card = ctk.CTkFrame(self.main_panel, fg_color="transparent")
        self.clock_card.grid(row=2, column=1, sticky="nsew", padx=24, pady=(14, 18))
        self.clock_card.grid_rowconfigure(1, weight=1)
        self.clock_card.grid_columnconfigure(0, weight=1)
        self.clock_header = self._create_card_header(self.clock_card, "CLOCK", "clock_22_simp.png")
        self.clock_header.grid(row=0, column=0, sticky="w", pady=(0, 12))
        self.clock_content = ctk.CTkFrame(self.clock_card, fg_color="transparent")
        self.clock_content.grid(row=1, column=0, sticky="nsew")
        self.clock_content.grid_rowconfigure(0, weight=1)
        self.clock_content.grid_columnconfigure(0, weight=1)
        self.clock_inner = ctk.CTkFrame(self.clock_content, fg_color="transparent")
        self.clock_inner.place(relx=0.0, rely=0.0, anchor="nw", x=0, y=0)
        self.clock_label = ctk.CTkLabel(
            self.clock_inner,
            text="00:00:00",
            text_color="#eaf1fb",
            font=("Segoe UI", 58, "bold"),
        )
        self.clock_label.pack(anchor="w", pady=(0, 8))
        self.date_label = ctk.CTkLabel(
            self.clock_inner,
            text="01/01/2026",
            text_color="#d7e2ef",
            font=("Segoe UI", 32, "bold"),
        )
        self.date_label.pack(anchor="w")

    def _set_clock_centered(self, centered: bool) -> None:
        """
        Adjusts the layout of the clock card to be either centered or left-aligned.
        Args:
            centered (bool): If True, the clock will be centered. If False, it will be left-aligned.
        This method updates the placement of the clock_inner frame and the alignment of the clock and date labels
        based on the specified centered parameter. When centered, the clock and date will be aligned to the center of the clock card. When not centered, they will be aligned to the left.
        """
        if centered:
            self.clock_inner.place(relx=0.5, rely=0.5, anchor="center")
            self.clock_label.pack_configure(anchor="center", pady=(0, 8))
            self.date_label.pack_configure(anchor="center")
            self.clock_label.configure(justify="center")
            self.date_label.configure(justify="center")
        else:
            self.clock_inner.place(relx=0.0, rely=0.0, anchor="nw", x=0, y=0)
            self.clock_label.pack_configure(anchor="w", pady=(0, 8))
            self.date_label.pack_configure(anchor="w")
            self.clock_label.configure(justify="left")
            self.date_label.configure(justify="left")

    def _build_side_buttons(self, parent):
        """
         Builds the side buttons for controlling the monitor view and pausing metrics.
         This method creates three buttons: one for cycling through view modes, one for
         toggling the pause state of the metrics, and one for returning to the home view mode.
         The buttons are styled consistently and arranged vertically on the right side of the window.
         Args:
             parent (ctk.CTkFrame): The parent frame for the side buttons.
         The buttons created are:
             - Top Button: Used for cycling through different view modes (e.g., Monitor, Performance, Clock).
             - Mid Button: Used for toggling the pause state of the metrics, allowing the user
               to freeze the current metric values on the screen.
             - Bottom Button: Used for returning to the default home view mode, which is typically the
               Monitor mode that shows all metrics.
         Each button is configured with a command that can be set externally to define the specific actions taken
         when the buttons are clicked.
        """
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
        """
        Sets the command for the top button.
        Args:
            function (callable): The function to be called when the top button is clicked.
        """
        self.btn_top.configure(command=function)

    def btn_mid_command(self, function) -> None:
        """
        Sets the command for the middle button.
        Args:
            function (callable): The function to be called when the middle button is clicked.
        """
        self.btn_mid.configure(command=function)

    def btn_bot_command(self, function) -> None:
        """
        Sets the command for the bottom button.
        Args:
            function (callable): The function to be called when the bottom button is clicked.
        """
        self.btn_bot.configure(command=function)

    def _apply_view_mode(self) -> None:
        """
        Applies the current view mode by showing or hiding the appropriate cards and adjusting the layout.
        This method checks the current view mode (monitor, performance, or clock) and updates the
        visibility and layout of the CPU, memory, GPU, and clock cards accordingly.
        It also updates the status label.
            The view modes are defined as follows:
                - Monitor Mode: Shows all cards (CPU, Memory, GPU, Clock) in a grid layout.
                - Performance Mode: Shows CPU and Memory cards in the top row and a combined 
                  GPU card in the bottom row.
                - Clock Mode: Shows only the Clock card, centered in the window.
        """
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
        """
        Cycles through the available view modes (monitor, performance, clock) in a circular manner.
            Each time this method is called, it updates the view_mode attribute to the next mode in the
            sequence and then applies the new view mode by calling the _apply_view_mode method.
        The order of the view modes is as follows:
            1. Monitor Mode: Shows all cards (CPU, Memory, GPU, Clock) in a grid layout.
            2. Performance Mode: Shows CPU and Memory cards in the top row and a combined 
               GPU card in the bottom row.
            3. Clock Mode: Shows only the Clock card, centered in the window.
        """
        order = ["monitor", "performance", "clock"]
        idx = order.index(self.view_mode)
        self.view_mode = order[(idx + 1) % len(order)]
        self._apply_view_mode()

    def toggle_metrics_pause(self) -> None:
        """
        Toggles the pause state of the metrics.
        When paused, the metrics will stop updating and the current values will be frozen on the screen.
        When unpaused, the metrics will resume updating with new data from the queue.
        This method updates the metrics_paused attribute and changes the text of the middle button to indicate
        whether the metrics are currently paused or active. It also updates the status label to reflect the
        current mode (paused or active).
        When the metrics are paused, the middle button will display ">" and the status label will show "MODE: PAUSED".
        When the metrics are active, the middle button will display "|||" and the status label will show the current view mode (e.g., "MODE: MONITOR", "MODE: PERFORMANCE", "MODE: CLOCK").
        """
        self.metrics_paused = not self.metrics_paused
        if self.metrics_paused:
            self.btn_mid.configure(text=">")
            self._set_status_mode("MODE: PAUSED")
        else:
            self.btn_mid.configure(text="|||")
            self._apply_view_mode()

    def go_home_mode(self) -> None:
        """
        sets the view mode to the default home mode, which is typically the Monitor mode that shows all metrics.
        This method updates the view_mode attribute to "monitor" and then applies the monitor view mode
        by calling the _apply_view_mode method. This allows the user to quickly return to the default view that 
        displays all hardware metrics.
        """
        self.view_mode = "monitor"
        self._apply_view_mode()

    def _on_exit(self) -> None:
        """
        Handles the window close event by calling the on_exit callback function and destroying the window.
        This method is called when the user attempts to close the window (e.g., by clicking the close button).
        It ensures that any necessary cleanup is performed by calling the provided on_exit function before
        destroying the window and exiting the application.
        The on_exit function is expected to handle tasks such as stopping background threads, closing resources, 
        and performing any other necessary cleanup to ensure a graceful shutdown of the application.
        """
        if self._on_exit_function:
            self._on_exit_function()
        self.destroy()

    def _get_latest_metrics(self) -> dict[str, float] | None:
        """
        Retrieves the latest metrics from the queue.
        This method attempts to get the most recent metrics data from the queue without blocking.
        If the queue is empty, it returns the last retrieved data or None if no data has been retrieved yet.
            Returns:
                dict[str, float] | None: A dictionary containing the latest metrics data, or None if no data is available.
        The expected format of the metrics data is a dictionary where the keys are metric names
        (e.g., "cpu_load", "gpu_temp") and the values are their corresponding float values.
        This method allows the MonitorPreview to update its displayed metrics with the most recent data received
        from a background thread or process that is responsible for collecting hardware metrics.
        """
        latest_data: dict[str, float] | None = None
        while True:
            try:
                latest_data = self._queue_metrics.get_nowait()
            except Empty:
                return latest_data

    def _update_metrics(self):
        """
        Updates the displayed metrics by retrieving the latest data from the queue 
        and applying it to the gauges and stat bars.
        Note:
            This method is called periodically (every _metrics_update_ms milliseconds)
            to refresh the displayed metrics.
            If the metrics are currently paused, it will skip updating and simply schedule the next update.
            If new metrics data is available from the queue, it will apply the measurements to the corresponding
            gauges and stat bars using the _apply_measurements method. After processing,
            it schedules the next update to continue refreshing the metrics display at regular intervals.
            The periodic updates ensure that the MonitorPreview window reflects the most current hardware metrics
            while allowing for pausing and resuming of updates as needed by the user.
        """
        if self.metrics_paused:
            self.after(self._metrics_update_ms, self._update_metrics)
            return
        data = self._get_latest_metrics()
        if data is not None:
            self._apply_measurements(data)
        self.after(self._metrics_update_ms, self._update_metrics)

    def _update_clock(self):
        """
        Updates the clock and date labels with the current time and date.
        Note:
            This method is called periodically (every 500 milliseconds) to refresh the displayed
            time and date. It ensures that the MonitorPreview window shows the most current
            time and date information.
        """
        now = datetime.now()
        self.clock_label.configure(text=now.strftime("%H:%M:%S"))
        self.date_label.configure(text=now.strftime("%d/%m/%Y"))
        self.after(500, self._update_clock)
