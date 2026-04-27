"""
Project: CardioIA - Go Beyond 1 (REST Email)
Linguage: Python
API: REST / Flask
AUTHOR: Raphael da Silva RM: 561452
GROUP: São Paulo e Interior
"""

# Import Libraries
from flask import Flask
from app.routes.vitals_routes import vitals_blueprint
from app.config import Config


def create_app() -> Flask:
    app = Flask(__name__)

    # Register the API routes.
    app.register_blueprint(vitals_blueprint)

    return app


if __name__ == "__main__":
    app = create_app()

    print(f"{Config.APP_NAME} started.")
    app.run(host="127.0.0.1", port=5000, debug=Config.DEBUG)