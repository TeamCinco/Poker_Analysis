# Poker Hand Trainer

A simple Streamlit app for training poker opening hand ranges.

## Setup

1. Install dependencies:
```bash
pip install -r requirements.txt
```

2. Run the app:
```bash
streamlit run app.py
```

## Mobile Access

To access from your mobile device:

1. When you run the app, Streamlit will show you a "Network URL" (usually something like `http://192.168.1.XXX:8501`)
2. Use this URL on your mobile device while connected to the same WiFi network
3. Or use the `--server.address 0.0.0.0` flag when running:
```bash
streamlit run app.py --server.address 0.0.0.0
```

## Features

- **Simple Interface**: Just two buttons - FOLD or PLAY
- **Hand Ranges**: Based on the provided poker hand charts
- **Score Tracking**: Shows your accuracy percentage
- **Mobile Friendly**: Large buttons and text for easy mobile use
- **No Accounts**: No login required, just start training immediately

## How to Use

1. Look at the displayed poker hand (e.g., "A-As", "K-Qo", "7-7")
2. Decide whether you should FOLD or PLAY this hand
3. Click the appropriate button
4. See if you were correct and learn the position context
5. Click "Next Hand" to continue
6. Use "Reset Training" to start over with fresh stats

The app will randomly show you hands from the training ranges and track your progress!
