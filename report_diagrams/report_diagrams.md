# Yukon Design Diagrams

## Figure 1: Overall Architecture

```mermaid
flowchart TD
    User["User"]

    Terminal["Terminal Interface"]
    GUI["SDL GUI Interface"]

    CommandHandler["Command Handler"]

    GameLogic["Game Logic"]
    MoveLogic["Move Validation & Execution"]
    ShuffleLogic["Shuffle Logic"]
    FileHandler["File Handler"]
    History["Undo / Redo History"]
    Timer["Timer"]

    Decks["decks/ folder<br/>Deck files"]
    Saves["saves/ folder<br/>Saved game states"]

    User --> Terminal
    User --> GUI

    Terminal --> CommandHandler
    GUI --> CommandHandler

    CommandHandler --> GameLogic
    CommandHandler --> MoveLogic
    CommandHandler --> ShuffleLogic
    CommandHandler --> FileHandler
    CommandHandler --> History
    CommandHandler --> Timer

    FileHandler --> Decks
    FileHandler --> Saves
```

## Figure 2: Game Phase Design

```mermaid
stateDiagram-v2
    [*] --> STARTUP

    STARTUP --> STARTUP: LD / SR / SI / SW / SD
    STARTUP --> PLAY: P
    STARTUP --> [*]: QQ

    PLAY --> PLAY: Move cards
    PLAY --> PLAY: U / R
    PLAY --> PLAY: S / L
    PLAY --> STARTUP: Q
    PLAY --> [*]: QQ
```

## Figure 3: Data Structure Design

```mermaid
classDiagram
    class Card {
        char rank
        char suit
        int face_up
        Card* next
    }

    class Pile {
        Card* top
    }

    class Deck {
        Card* deck
    }

    class Columns {
        Pile columns[7]
    }

    class Foundations {
        Pile foundations[4]
    }

    Deck --> Card
    Columns --> Pile
    Foundations --> Pile
    Pile --> Card
    Card --> Card : next
```

## Figure 4: Deck Save Design

```mermaid
flowchart TD
    Start["User enters SD command"]

    HasArg{"Filename argument?"}
    UseArg["Use provided filename"]
    UseCurrent["Use currentFile"]

    AddTxt{"Ends with .txt?"}
    AppendTxt["Append .txt"]
    BuildPath["Build path: decks/filename.txt"]

    EnsureDecks{"Does decks/ exist?"}
    CreateDecks["Create decks/ folder"]
    OpenFile["Open file with write mode"]
    WriteCards["Write 52 cards"]
    Done["Deck saved"]

    Start --> HasArg
    HasArg -->|Yes| UseArg
    HasArg -->|No| UseCurrent

    UseArg --> AddTxt
    UseCurrent --> BuildPath

    AddTxt -->|Yes| BuildPath
    AddTxt -->|No| AppendTxt
    AppendTxt --> BuildPath

    BuildPath --> EnsureDecks
    EnsureDecks -->|No| CreateDecks
    EnsureDecks -->|Yes| OpenFile
    CreateDecks --> OpenFile

    OpenFile --> WriteCards
    WriteCards --> Done
```

## Figure 5: Deck File vs Save File

```mermaid
flowchart LR
    DeckFile["Deck file<br/>decks/name.txt"]
    SaveFile["Save file<br/>saves/name.txt"]

    DeckFile --> DeckData["Stores only card order<br/>52 cards"]
    SaveFile --> StateData["Stores full game state<br/>columns, foundations,<br/>face-up cards, timer, phase"]

    DeckData --> LD["Loaded with LD"]
    StateData --> L["Loaded with L"]

    DeckData --> SD["Created with SD"]
    StateData --> S["Created with S"]
```

## Figure 6: Shared Backend Between Terminal and GUI

```mermaid
sequenceDiagram
    participant User
    participant Terminal
    participant GUI
    participant Handler as Command Handler
    participant Backend as Shared Backend Logic

    User->>Terminal: Type command, e.g. SD
    Terminal->>Handler: Send command string

    User->>GUI: Click Save Deck
    GUI->>Handler: Send command string, e.g. SD gui_deck.txt

    Handler->>Backend: Execute same save logic
    Backend-->>Handler: Result message
    Handler-->>Terminal: Display message
    Handler-->>GUI: Display/update state
```
