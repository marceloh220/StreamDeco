from modules.report import report


def btn_top_action(app) -> None:
    """
    Cycles the view mode in the monitor UI.
    """
    app.cycle_view_mode()
    report(
        "StreamDecoCustomTkinterPreview",
        "INFO",
        "Top button clicked! View mode cycled.",
    )


def btn_mid_action(app) -> None:
    """
    Toggles metrics pause/play in the monitor UI.
    """
    app.toggle_metrics_pause()
    report(
        "StreamDecoCustomTkinterPreview",
        "INFO",
        "Mid button clicked! Metrics pause toggled.",
    )


def btn_bot_action(app) -> None:
    """
    Activates home mode in the monitor UI.
    """
    app.go_home_mode()
    report(
        "StreamDecoCustomTkinterPreview",
        "INFO",
        "Bot button clicked! Home mode activated.",
    )
