"""
Project: CardioIA - Go Beyond 1 (REST Email)
Linguage: Python
API: REST / Flask
AUTHOR: Raphael da Silva RM: 561452
GROUP: São Paulo e Interior
"""

# Import libraries
import json
import os
from app.config import Config

class AlertLogService:
    @staticmethod
    def save_alert(vital_signs: dict, risk_result: dict, email_sent: bool) -> None:
        # Ensures that the data folder exists.
        os.makedirs(os.path.dirname(Config.ALERT_LOG_PATH), exist_ok=True)

        log_entry = {
            "vital_signs": vital_signs,
            "risk_result": risk_result,
            "email_sent": email_sent
        }

        logs = []

        if os.path.exists(Config.ALERT_LOG_PATH):
            try:
                with open(Config.ALERT_LOG_PATH, "r", encoding="utf-8") as file:
                    logs = json.load(file)
            except json.JSONDecodeError:
                logs = []

        logs.append(log_entry)

        with open(Config.ALERT_LOG_PATH, "w", encoding="utf-8") as file:
            json.dump(logs, file, indent=4, ensure_ascii=False)