"""
Project: CardioIA - Go Beyond 1 (REST Email)
Linguage: Python
API: REST / Flask
AUTHOR: Raphael da Silva RM: 561452
GROUP: São Paulo e Interior
"""

# import libraries
import os
from dotenv import load_dotenv

load_dotenv()

class Config:
    # General Configurations
    APP_NAME = "CardioIA REST Monitoring API"
    DEBUG = True

    # Simulated clinical limits for demonstration
    MAX_TEMPERATURE_C = 38.0
    MAX_HEART_RATE_BPM = 120
    MAX_NO_MOVIMENT_SECONDS = 60

    # Email sending mode
    # True = only simulates in the terminal
    # False = attempts to send a real email via SMTP
    EMAIL_SIMULATION_MODE = os.getenv("EMAIL_SIMULATION_MODE", "true").lower() == "true"

    # SMTP configuration
    SMTP_HOST = os.getenv("SMTP_HOST", "smtp.gmail.com")
    SMTP_PORT = int(os.getenv("SMTP_PORT", "587"))
    SMTP_USER = os.getenv("SMTP_USER", "")
    SMTP_PASSWORD = os.getenv("SMTP_PASSWORD", "")
    ALERT_RECEIVER_EMAIL = os.getenv("ALERT_RECEIVER_EMAIL", "responsive@exemplo.com")

    # Log file configuration
    ALERT_LOG_PATH = "data/alerts_log.json"