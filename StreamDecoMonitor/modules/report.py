
import sys

_debugLevels = {
    "ERROR": 0,
    "WARNING": 1,
    "INFO": 2,
    "DEBUG": 3
}

_debugLevel = 0


def set_debug_level(level: str) -> None:
    """Set the debug level for reporting.
    Args:
        level (str): The debug level to set. Valid levels are "ERROR", "WARNING", "INFO", "DEBUG".
    """
    global _debugLevel
    if level in _debugLevels:
        _debugLevel = _debugLevels[level]
        report("Report", "INFO", f"Debug level set to {level} ({_debugLevel})")
    else:
        report("Report", "ERROR", f"Invalid debug level: {level}. Valid levels are: {', '.join(_debugLevels.keys())}")

def get_debug_level() -> str:
    """Get the current debug level as a string.
    Returns:
        str: The current debug level ("ERROR", "WARNING", "INFO", "DEBUG").
    """
    global _debugLevel
    for key, value in _debugLevels.items():
        if value == _debugLevel:
            return key
    return "UNKNOWN"

def report(reporter: str, type: str, message: str) -> str:
    """
    Reports a message with a specified type and reporter.
    Args:
        reporter (str): The name of the reporter or module generating the message.
        type (str): The type of the message ("ERROR", "WARNING", "INFO", "DEBUG").
        message (str): The message content to report.
    Returns:
        str: The formatted report message.
    """
    global _debugLevel
    reportmsg = f" {reporter} [{type}] {message}"
    if type in _debugLevels:
        if _debugLevel >= _debugLevels[type]:
             print(reportmsg, file=sys.stderr if type == "ERROR" else sys.stdout)
    else:
        print(f" Report [ERROR] Invalid report type: {type}. Message: {message}", file=sys.stderr)
    return reportmsg