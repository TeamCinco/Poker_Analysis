# Stream server
# Handles live streaming functionality
import asyncio
import websockets
import json
from datetime import datetime
from typing import Set, Dict, Any

class StreamServer:
    def __init__(self, host: str = "localhost", port: int = 8765):
        self.host = host
        self.port = port
        self.clients: Set[websockets.WebSocketServerProtocol] = set()
        self.server = None
        self.is_running = False
    
    async def register_client(self, websocket: websockets.WebSocketServerProtocol):
        """Register a new client"""
        self.clients.add(websocket)
        print(f"Client connected. Total clients: {len(self.clients)}")
        
        # Send welcome message
        await websocket.send(json.dumps({
            "type": "connection_established",
            "message": "Connected to Poker Range Trainer Stream",
            "timestamp": datetime.now().isoformat()
        }))
    
    async def unregister_client(self, websocket: websockets.WebSocketServerProtocol):
        """Unregister a client"""
        self.clients.discard(websocket)
        print(f"Client disconnected. Total clients: {len(self.clients)}")
    
    async def handle_client(self, websocket: websockets.WebSocketServerProtocol, path: str):
        """Handle individual client connection"""
        await self.register_client(websocket)
        
        try:
            async for message in websocket:
                data = json.loads(message)
                await self.process_message(websocket, data)
        except websockets.exceptions.ConnectionClosed:
            pass
        finally:
            await self.unregister_client(websocket)
    
    async def process_message(self, websocket: websockets.WebSocketServerProtocol, data: Dict[str, Any]):
        """Process incoming messages from clients"""
        message_type = data.get("type")
        
        if message_type == "training_update":
            # Broadcast training data to all connected clients
            await self.broadcast_training_data(data)
        elif message_type == "request_stats":
            # Send current stats to requesting client
            await self.send_stats(websocket)
        elif message_type == "ping":
            # Respond to ping
            await websocket.send(json.dumps({"type": "pong", "timestamp": datetime.now().isoformat()}))
    
    async def broadcast_training_data(self, data: Dict[str, Any]):
        """Broadcast training data to all connected clients"""
        if self.clients:
            message = json.dumps({
                "type": "training_broadcast",
                "data": data,
                "timestamp": datetime.now().isoformat()
            })
            
            # Send to all connected clients
            disconnected_clients = set()
            for client in self.clients:
                try:
                    await client.send(message)
                except websockets.exceptions.ConnectionClosed:
                    disconnected_clients.add(client)
            
            # Remove disconnected clients
            self.clients -= disconnected_clients
    
    async def send_stats(self, websocket: websockets.WebSocketServerProtocol):
        """Send current session stats to client"""
        stats = {
            "connected_clients": len(self.clients),
            "server_uptime": datetime.now().isoformat(),
            "stream_status": "active" if self.is_running else "inactive"
        }
        
        await websocket.send(json.dumps({
            "type": "stats_response",
            "stats": stats,
            "timestamp": datetime.now().isoformat()
        }))
    
    def start(self):
        """Start the WebSocket server"""
        self.is_running = True
        self.server = websockets.serve(self.handle_client, self.host, self.port)
        
        print(f"Poker Range Trainer Stream Server starting on {self.host}:{self.port}")
        
        # Run the server
        asyncio.get_event_loop().run_until_complete(self.server)
        asyncio.get_event_loop().run_forever()
    
    def stop(self):
        """Stop the WebSocket server"""
        self.is_running = False
        if self.server:
            self.server.close()
            print("Stream server stopped")