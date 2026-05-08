# Yukon Report Diagrams

This folder contains Mermaid diagrams for the report design section.

- `report_diagrams.md` contains all diagrams in one Markdown file.
- `01_overall_architecture.mmd` explains the shared backend design.
- `02_game_phase_design.mmd` explains STARTUP and PLAY command separation.
- `03_data_structure_design.mmd` explains the linked-list card and pile model.
- `04_deck_save_design.mmd` explains the `SD` save flow.
- `05_deck_file_vs_save_file.mmd` explains why `decks/` and `saves/` are separate.
- `06_shared_backend_sequence.mmd` explains how terminal and GUI actions reach the same backend logic.

If your report tool supports Mermaid, paste the code directly.
If it does not, open the diagrams in a Mermaid renderer and export them as PNG or SVG before inserting them.
