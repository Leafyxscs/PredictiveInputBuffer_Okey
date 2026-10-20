# Predictive Input Buffer (console status build)

This source-project attempt is based on the public `johanxwf/predictive-input-buffer` recreation. It uses **O** to cycle stages and prints the current stage plus general/wave buffer values to the **Geode console**. It does **not** add an in-game HUD.

When O is pressed, the console receives a clearly separated status block for the selected stage. The cycle is Stage 1 (2/2 ticks), Stage 2 (4/4), Stage 3 (8/8), Stage 4 (16/8), then OFF.

## Important
- This is a source project, **not a compiled `.geode` file**. It has not been compiled or tested in Geometry Dash in this environment.
- The Geode console/log window must be available to see the status output.
- The upstream recreation is not the original developer's source. SDK/API or gameplay-hook differences may require fixes.

## Build
1. Upload the contents of this folder to a new GitHub repository.
2. Open **Actions** and run **Build Geode Mod**.
3. Download the **Build Output** artifact if the workflow succeeds.
4. Test with a backup of your existing mod.
