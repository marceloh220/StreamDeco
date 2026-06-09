from queue import Queue

from modules.report import report
from modules.serial_sender_task import SerialSenderTask
from modules.sound_notification import play_reconnect_failure_sound


def connect_serial_device(
    *,
    app,
    serial_sender_state: dict[str, SerialSenderTask | None],
    queue_serial_sender: Queue[str],
    new_port: str,
    source: str,
) -> bool:
    """
    Connects (or reconnects) the serial sender using the selected COM port.
    """
    app.show_status_notice("SERIAL: conectando...", severity="info", duration_ms=1500)
    current_sender = serial_sender_state["task"]
    if current_sender is None:
        # Create and start a new sender if no sender exists yet.
        new_sender = SerialSenderTask(
            boardCOM=new_port,
            queue_serial_sender=queue_serial_sender,
            run_task=True,
            update_interval_seconds=1.0,
        )
        new_sender.start()
        serial_sender_state["task"] = new_sender
        report(
            "StreamDecoCustomTkinterPreview",
            "INFO",
            f"[{source}] Created new serial sender for {new_port}.",
        )

    else:
        # Reuse the existing sender thread and only rotate the COM port.
        report(
            "StreamDecoCustomTkinterPreview",
            "INFO",
            f"[{source}] Switching serial sender port to {new_port}.",
        )
        current_sender.set_port(new_port)
        if not current_sender.is_running():
            current_sender.start()

    report(
        "StreamDecoCustomTkinterPreview",
        "INFO",
        f"[{source}] Reconnect successful on {new_port}.",
    )
    app.show_status_notice(f"SERIAL: conectado em {new_port}", severity="success")
    return True


def reconnect_device(
    *,
    app,
    search_com_port,
    connect_serial_device_fn,
) -> None:
    """
    Callback to manually reconnect the serial device from the system tray.
    """
    report("StreamDecoCustomTkinterPreview", "INFO", "Manual reconnect requested from system tray.")
    app.show_status_notice("SERIAL: reconectando...", severity="info", duration_ms=1500)

    # Search for a compatible COM port again in case it was reconnected.
    new_port = search_com_port()
    if not new_port:
        # If no compatible COM port is found, keep current sender unchanged and notify the user.
        report("StreamDecoCustomTkinterPreview", "ERROR", "Reconnect failed: no compatible COM port found.")
        play_reconnect_failure_sound()
        app.show_status_notice("SERIAL: falha ao reconectar", severity="error")
        return

    connect_serial_device_fn(new_port, source="tray")
