# Tank Wars 3D

A multiplayer 3D tank battle game written in C for the [WASM-4](https://wasm4.org) fantasy console. Features real-time 3D rendering and netplay support for up to 4 players.

You can give it a try here: [Tank Wars](https://tank.coderoller.org).

## Game Controls

### Menu Navigation
- **Arrow Keys**: Navigate menus and select options
- **X Button**: Confirm selections and start game
- **Z Button**: Go back to previous menu

### In-Game Controls
- **Arrow Keys**: Move tank forward/backward and turn left/right
- **Z Button**: Precise turning (slower rotation)
- **X Button**: Fire projectiles (3-second cooldown)

## Building and Running

### Prerequisites
- [WASI SDK](https://github.com/WebAssembly/wasi-sdk) - Download and set `WASI_SDK_PATH` environment variable
- [WASM-4 CLI](https://wasm4.org/docs/getting-started/setup) - Install the `w4` command

### Build Commands

**Build the game:**
```shell
make
```

**Run the game:**
```shell
w4 run build/cart.wasm
```

**Run with live reload during development:**
```shell
w4 watch
```

**Clean build artifacts:**
```shell
make clean
```

**Run code linting:**
```shell
make lint
```

## Development

### Project Structure
```
src/
├── main.c      # Main game loop and core logic
├── menu.c/h    # Menu system and UI
├── render.c/h  # 3D rendering pipeline
├── object.c/h  # Game object management
├── models.c/h  # 3D model definitions
├── draw.c/h    # Drawing utilities
├── io.c        # Input/output handling
└── wasm4.h     # WASM-4 API definitions
```

### Debug vs Release
- **Debug build**: `make DEBUG=1` - Includes debug symbols and optimizations disabled
- **Release build**: `make` (default) - Optimized for size and performance

## Gameplay

1. **Main Menu**: Select "New Game" or "Help"
2. **Player Selection**: Choose 2-4 players for the match
3. **Battle**: Players spawn in different corners of the arena
4. **Objective**: Hit other tanks with projectiles to score points
5. **Victory**: First player to reach 10 points wins
6. **Return**: Game automatically returns to menu after victory screen

## Netplay

The game supports WASM-4's built-in netplay system for 2-4 players.

## Technical Details

- **Engine**: Custom 3D rendering engine with matrix transformations
- **Graphics**: 160x160 pixel display with 4-color palette
- **Performance**: 60 FPS target with optimized polygon rendering
- **Memory**: Fits within WASM-4's 64KB memory limit
- **Audio**: Uses WASM-4's tone generator for sound effects

## Links

- [WASM-4 Documentation](https://wasm4.org/docs): Learn more about the fantasy console
- [WASM-4 GitHub](https://github.com/aduros/wasm4): Platform source code and issues
