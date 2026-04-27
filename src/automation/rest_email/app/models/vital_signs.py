"""
Project: CardioIA - Go Beyond 1 (REST Email)
Linguage: Python
API: REST / Flask
AUTHOR: Raphael da Silva RM: 561452
GROUP: São Paulo e Interior
"""

# Import libraries
from dataclasses import dataclass
from datetime import datetime
from typing import Optional

@dataclass
class VitalSigns:
    device_id: str
    patient_id: str
    temperature_c: float
    heart_rate_bpm: int
    movement_detected: bool
    emergency_button_pressed: bool
    timestamp: Optional[str] = None

    @classmethod
    def from_dict(cls, data: dict):
        # Converts the JSON received from the API into a structured object.
        return cls(
            device_id=data.get("device_id", "unknown_device"),
            patient_id=data.get("patient_id", "unknown_patient"),
            temperature_c=float(data.get("temperature_c", 0.0)),
            heart_rate_bpm=int(data.get("heart_rate_bpm", 0)),
            movement_detected=bool(data.get("movement_detected", False)),
            emergency_button_pressed=bool(data.get("emergency_button_pressed", False)),
            timestamp=data.get("timestamp") or datetime.now().isoformat()
        )
    
    def to_dict(self) -> dict:
        # Converts the object to a dictionary, facilitating JSON return and logs.
        return {
            "device_id": self.device_id,
            "patient_id": self.patient_id,
            "temperature_c": self.temperature_c,
            "heart_rate_bpm": self.heart_rate_bpm,
            "movement_detected": self.movement_detected,
            "emergency_button_pressed": self.emergency_button_pressed,
            "timestamp": self.timestamp 
        }

