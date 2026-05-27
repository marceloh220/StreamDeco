import sys
from pathlib import Path
from .report import report

if sys.platform == "win32":
    try:
        import winsound
    except Exception:
        winsound = None

RECONNECT_FAILURE_WAV = "reconnect_failure.wav"

def _assets_dir() -> Path:
    if getattr(sys, "frozen", False):
        return Path(getattr(sys, "_MEIPASS")) / "assets"
    return Path(__file__).resolve().parent / "assets"


def play_reconnect_failure_sound() -> None:
    """
    Plays a short Windows error sound when reconnect fails.
    """
    if winsound is None:
        return
    try:
        custom_wav_path = _assets_dir() / RECONNECT_FAILURE_WAV
        if custom_wav_path.exists():
            winsound.PlaySound(
                str(custom_wav_path),
                winsound.SND_FILENAME | winsound.SND_ASYNC | winsound.SND_NODEFAULT,
            )
            return
        winsound.MessageBeep(winsound.MB_ICONHAND)
    except Exception as exc:
        report("StreamDecoCustomTkinterPreview", "WARNING", f"Failed to play reconnect error sound: {exc}")

