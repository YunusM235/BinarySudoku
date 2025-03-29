#include "colours.h"
#include "solvePuzzle.h"
#include <algorithm>
#include <random>
#include <map>
#include <vector>
#include "raylib.h"
#include <iostream>
#include <filesystem>




int xPos(int n, int column){
    return 280+(column-1)*(720/n);
}

int yPos(int n, int row){
    return 40+(row-1)*(720/n);
}

bool isInsideCell(int n, int row, int column, int x, int y){
    if (x<xPos(n, column) || x>=xPos(n, column+1)) return false;
    if (y<yPos(n, row) || y>=yPos(n, row+1)) return false;
    return true;
}

void drawBoard(Board &board, const std::vector<bool> &cluesPos, int row, int column, bool inPauseScreen, int boardSelectedRow, int boardSelectedColumn){
    int n = board.rank();

    static const Texture2D border6{LoadTexture("textures/border6.png")};
    static const Texture2D border6_2{LoadTexture("textures/border6_2.png")};
    static const Texture2D border6_3{LoadTexture("textures/border6_3.png")};
    static const Texture2D border8{LoadTexture("textures/border8.png")};
    static const Texture2D border8_2{LoadTexture("textures/border8_2.png")};
    static const Texture2D border8_3{LoadTexture("textures/border8_3.png")};
    static const Texture2D border10{LoadTexture("textures/border10.png")};
    static const Texture2D border10_2{LoadTexture("textures/border10_2.png")};
    static const Texture2D border10_3{LoadTexture("textures/border10_3.png")};
    static const Texture2D solved{LoadTexture("textures/solved.png")};

    Texture2D border1;
    Texture2D border2;
    Texture2D border3;
    int width;
    int offset;


    if (n==6){
        border1 = border6;
        border2 = border6_2;
        border3 = border6_3;
        width = 102;
        offset = 9;
    } else if (n==8) {
        border1 = border8;
        border2 = border8_2;
        border3 = border8_3;
        width = 77;
        offset = 7;
    } else if (n==10) {
        border1 = border10;
        border2 = border10_2;
        border3 = border10_3;
        width = 62;
        offset = 6;
    }

    static Color color1{Color{130, 109, 93, 255}};
    static Color color2{Color{129,98,113,255}};
    static std::map<Colour::Colour, Color> colorMapping = {{Colour::black, color1}, {Colour::white, color2}, {Colour::none, BLANK}};

    bool boardSolved = isBoardSolved(board);
    if (boardSolved) DrawTexture(solved, 400, 280, RAYWHITE);
    DrawRectangle(xPos(n, column)+offset, yPos(n, row)+offset, width, width, colorMapping[board.valueRC(row, column)]);
    if (!boardSolved && ((IsGamepadAvailable(0) && boardSelectedRow==row && boardSelectedColumn==column) ||isInsideCell(n, row, column, GetMouseX(), GetMouseY())) && !cluesPos[n*(row-1)+column-1] && !inPauseScreen){
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
            if (board.valueRC(row, column)==Colour::none) board.setValueRC(row, column, Colour::black);
            else if (board.valueRC(row, column)==Colour::black) board.setValueRC(row, column, Colour::white);
            else if (board.valueRC(row, column)==Colour::white) board.setValueRC(row, column, Colour::none);
            DrawRectangle(xPos(n, column)+offset, yPos(n, row)+offset, width, width, colorMapping[board.valueRC(row, column)]);

        }
        DrawTexture(border2, xPos(n, column), yPos(n, row), RAYWHITE);
    } else DrawTexture(border1, xPos(n, column), yPos(n, row), RAYWHITE);

    if (IsGamepadAvailable(0) && boardSelectedRow==row && boardSelectedColumn==column && cluesPos[n*(row-1)+column-1]) {
        DrawTexture(border3, xPos(n, column), yPos(n, row), RAYWHITE);
    }


}


enum GameScreen {
    TitleScreen,
    BoardScreen,
    RulesScreen,
};



void fadeToBlack(bool &isFadingToBlack, GameScreen &currentScreen, bool &isUnfadingToBlack, GameScreen fadingTo){
    static int fadeFrameCounter;
    DrawRectangle(0, 0, 1280, 800, Color{0, 0, 0, static_cast<unsigned char>(15*fadeFrameCounter)});
    fadeFrameCounter++;
    if (fadeFrameCounter==17){
        isFadingToBlack = false;
        fadeFrameCounter = 0;
        currentScreen = fadingTo;
        isUnfadingToBlack = true;
    }
}

void unfadeFromBlack(bool &isUnfadingFromBlack ){
    static int fadeFrameCounter{0};
    DrawRectangle(0, 0, 1280, 800, Color{0, 0, 0, static_cast<unsigned char>(255-15*fadeFrameCounter)});
    fadeFrameCounter++;
    if (fadeFrameCounter==17){
        fadeFrameCounter = 0;
        isUnfadingFromBlack = false;
    }
}

//TODO make the main function less ugly and put drawBoard in main
int main(){

    GameScreen currentScreen = TitleScreen;

    InitWindow(1280, 800, "BinarySudoku");
    SetWindowIcon(LoadImage("icon.png"));
    SetTargetFPS(60);
    InitAudioDevice();

    std::vector<Music> musicFiles{};
    for (const auto & entry : std::filesystem::directory_iterator("music")){
        if (entry.path().extension()!=".wav") continue;
        musicFiles.push_back(LoadMusicStream(entry.path().string().c_str()));
    }
    std::mt19937 mt{ std::random_device{}() };
    std::shuffle(musicFiles.begin(), musicFiles.end(), mt);

    int currMusic = 0;
    Music currMusicFile = musicFiles[currMusic];
    float musicTimePlayed{};
    PlayMusicStream(currMusicFile);
    SetMasterVolume(0.5);
    bool soundOn{true};

    const Texture2D backgroundBoard{LoadTexture("textures/background.png")};
    const Texture2D backgroundMenu{LoadTexture("textures/titleScreen.png")};
    const Texture2D rules{LoadTexture("textures/rules.png")};
    const Texture2D button6{LoadTexture("textures/button6.png")};
    const Texture2D button6_2{LoadTexture("textures/button6_2.png")};
    const Texture2D button8{LoadTexture("textures/button8.png")};
    const Texture2D button8_2{LoadTexture("textures/button8_2.png")};
    const Texture2D button10{LoadTexture("textures/button10.png")};
    const Texture2D button10_2{LoadTexture("textures/button10_2.png")};
    const Texture2D buttonHelp{LoadTexture("textures/buttonHelp.png")};
    const Texture2D buttonHelp_2{LoadTexture("textures/buttonHelp_2.png")};
    const Texture2D buttonHome{LoadTexture("textures/buttonHome.png")};
    const Texture2D buttonHome_2{LoadTexture("textures/buttonHome_2.png")};
    const Texture2D buttonSoundOn{LoadTexture("textures/buttonSoundOn.png")};
    const Texture2D buttonSoundOn_2{LoadTexture("textures/buttonSoundOn_2.png")};
    const Texture2D buttonSoundOff{LoadTexture("textures/buttonSoundOff.png")};
    const Texture2D buttonSoundOff_2{LoadTexture("textures/buttonSoundOff_2.png")};
    const Texture2D buttonLeave{LoadTexture("textures/leave.png")};
    const Texture2D buttonLeave_2{LoadTexture("textures/leave_2.png")};
    const Texture2D pauseWindow{LoadTexture("textures/pauseWindow.png")};

    bool inPauseScreen{false};
    bool isFadingToBlack{false};
    bool isUnfadingFromBlack{false};
    GameScreen fadingTo;

    int n;
    Board board{0};
    std::vector<bool> cluesPos{};

    SetExitKey(KEY_F1);

    bool gamepadUsed = IsGamepadAvailable(0);

    int menuSelectedButton = 1;
    int settingsSelectedButton = 1;
    int boardSelectedRow = 1;
    int boardSelectedColumn = 1;

    bool closeWindow{false};

    while(!closeWindow){
        UpdateMusicStream(currMusicFile);
        BeginDrawing();

        switch (currentScreen){
            case GameScreen::TitleScreen: {
                DrawTexture(backgroundMenu, 0, 0, RAYWHITE);
                if ((GetMouseX()>500 && GetMouseX() < 575 && GetMouseY()>415 && GetMouseY()<490) || (IsGamepadAvailable(0) && menuSelectedButton == 1)){
                    DrawTexture(button6_2, 500, 415, RAYWHITE);
                    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) && !isFadingToBlack){
                        isFadingToBlack = true;
                        fadingTo = BoardScreen;
                        boardSelectedRow = 1;
                        boardSelectedColumn = 1;
                        n = 6;
                    }
                } else DrawTexture(button6, 500, 415, RAYWHITE);

                if (GetMouseX()>600 && GetMouseX() < 675 && GetMouseY()>415 && GetMouseY()<490 || (IsGamepadAvailable(0) && menuSelectedButton == 2)){
                    DrawTexture(button8_2, 600, 415, RAYWHITE);
                    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) && !isFadingToBlack){
                        isFadingToBlack = true;
                        fadingTo = BoardScreen;
                        boardSelectedRow = 1;
                        boardSelectedColumn = 1;
                        n = 8;
                    }
                } else DrawTexture(button8, 600, 415, RAYWHITE);

                if (GetMouseX()>700 && GetMouseX() < 775 && GetMouseY()>415 && GetMouseY()<490 || (IsGamepadAvailable(0) && menuSelectedButton == 3)){
                    DrawTexture(button10_2, 700, 415, RAYWHITE);
                    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) && !isFadingToBlack){
                        isFadingToBlack = true;
                        fadingTo = BoardScreen;
                        boardSelectedRow = 1;
                        boardSelectedColumn = 1;
                        n = 10;
                    }
                } else DrawTexture(button10, 700, 415, RAYWHITE);

                if (GetMouseX()>552 && GetMouseX() < 627 && GetMouseY()>535 && GetMouseY()<610 || (IsGamepadAvailable(0) && menuSelectedButton == 4)){
                    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) && !isFadingToBlack && !isUnfadingFromBlack){
                        isFadingToBlack = true;
                        fadingTo = RulesScreen;
                    }
                    DrawTexture(buttonHelp_2, 552, 535, RAYWHITE);
                } else DrawTexture(buttonHelp, 552, 535, RAYWHITE);

                if (GetMouseX()>652 && GetMouseX() < 727 && GetMouseY()>535 && GetMouseY()<610 || (IsGamepadAvailable(0) && menuSelectedButton == 5)){
                    if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) && !isFadingToBlack && !isUnfadingFromBlack){
                        closeWindow = true;
                    }
                    DrawTexture(buttonLeave_2, 652, 535, RAYWHITE);
                } else DrawTexture(buttonLeave, 652, 535, RAYWHITE);

                if (isFadingToBlack){
                    fadeToBlack(isFadingToBlack, currentScreen, isUnfadingFromBlack, fadingTo);
                    if (!isFadingToBlack && fadingTo==BoardScreen) {
                        board = createPuzzle(n);
                        cluesPos = std::vector<bool>(n*n, false);
                        for (int i=0;i<n*n;i++){
                            if (board.value(i)!=Colour::none) cluesPos[i] = true;
                        }
                    }
                } else if (isUnfadingFromBlack){
                    unfadeFromBlack(isUnfadingFromBlack);
                }

                if (menuSelectedButton==1 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) menuSelectedButton = 2;
                else if (menuSelectedButton==2 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) menuSelectedButton = 3;
                else if (menuSelectedButton==4 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) menuSelectedButton = 5;
                if (menuSelectedButton==2 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) menuSelectedButton = 1;
                else if (menuSelectedButton==3 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) menuSelectedButton = 2;
                else if (menuSelectedButton==5 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) menuSelectedButton = 4;
                if ((menuSelectedButton == 1 || menuSelectedButton == 2) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) menuSelectedButton = 4;
                else if (menuSelectedButton == 3 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) menuSelectedButton = 5;
                if (menuSelectedButton==4 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) menuSelectedButton = 1;
                else if (menuSelectedButton==5 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) menuSelectedButton = 3;

            }
            break;
            case GameScreen::BoardScreen: {
                DrawTexture(backgroundBoard, 0, 0, RAYWHITE);

                for (int row=1;row<=n;row++){
                    for (int column=1;column<=n;column++){
                        drawBoard(board, cluesPos, row, column, inPauseScreen, boardSelectedRow, boardSelectedColumn);
                    }
                }
                if (isFadingToBlack){
                    DrawRectangle(0, 0, 1280, 800, Color{0, 0, 0, 200});
                    DrawTexture(pauseWindow, 465, 337, RAYWHITE);
                    DrawTexture(buttonHome, 540, 362, RAYWHITE);
                    if (soundOn) DrawTexture(buttonSoundOn, 665, 362, RAYWHITE);
                    else DrawTexture(buttonSoundOff, 665, 362, RAYWHITE);
                    fadeToBlack(isFadingToBlack, currentScreen, isUnfadingFromBlack, fadingTo);
                    if (!isFadingToBlack) inPauseScreen = false;
                    break;
                } else if (isUnfadingFromBlack){
                    unfadeFromBlack(isUnfadingFromBlack);
                    break;
                }

                if (!inPauseScreen){
                    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP) && boardSelectedRow!=1) boardSelectedRow--;
                    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) && boardSelectedRow!=n) boardSelectedRow++;
                    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) && boardSelectedColumn!=1) boardSelectedColumn--;
                    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) && boardSelectedColumn!=n) boardSelectedColumn++;
                }

                if (IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    if (inPauseScreen) {
                        inPauseScreen = false;
                        settingsSelectedButton = 1;
                    } else inPauseScreen = true;
                }
                if (inPauseScreen) {
                    DrawRectangle(0, 0, 1280, 800, Color{0, 0, 0, 200});
                    DrawTexture(pauseWindow, 465, 337, RAYWHITE);
                    if ((GetMouseX()>540 && GetMouseX() <615 && GetMouseY() > 362 && GetMouseY() < 437) || (IsGamepadAvailable(0) && settingsSelectedButton == 1)){
                        DrawTexture(buttonHome_2, 540, 362, RAYWHITE);
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
                            isFadingToBlack = true;
                            fadingTo = TitleScreen;
                            menuSelectedButton = 1;
                        }
                    } else DrawTexture(buttonHome, 540, 362, RAYWHITE);
                    if ((GetMouseX()>665 && GetMouseX() <740 && GetMouseY() > 362 && GetMouseY() < 437) || (IsGamepadAvailable(0) && settingsSelectedButton == 2)){
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)){
                            if (soundOn) {
                                soundOn = false;
                                SetMasterVolume(0);
                            } else {
                                soundOn = true;
                                SetMasterVolume(0.5);
                            }
                        }
                        if (soundOn) DrawTexture(buttonSoundOn_2, 665, 362, RAYWHITE);
                        else DrawTexture(buttonSoundOff_2, 665, 362, RAYWHITE);
                    } else {
                        if (soundOn) DrawTexture(buttonSoundOn, 665, 362, RAYWHITE);
                        else DrawTexture(buttonSoundOff, 665, 362, RAYWHITE);
                    }
                    if (settingsSelectedButton == 1 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) settingsSelectedButton = 2;
                    else if (settingsSelectedButton == 2 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) settingsSelectedButton = 1;
                }
            }
            break;
            case GameScreen::RulesScreen: {
                DrawTexture(rules, 0, 0, RAYWHITE);
                if (isFadingToBlack) {
                    fadeToBlack(isFadingToBlack, currentScreen, isUnfadingFromBlack, fadingTo);
                } else if (isUnfadingFromBlack){
                    unfadeFromBlack(isUnfadingFromBlack);
                } else if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
                    isFadingToBlack = true;
                    fadingTo = TitleScreen;
                }
            }
            break;
        }

        EndDrawing();

        if (!(GetMusicTimePlayed(currMusicFile)<musicTimePlayed)) musicTimePlayed = GetMusicTimePlayed(currMusicFile);
        if (GetMusicTimePlayed(currMusicFile)<musicTimePlayed || GetMusicTimePlayed(currMusicFile)>=GetMusicTimeLength(currMusicFile)){
            musicTimePlayed = 0;
            StopMusicStream(currMusicFile);
            if (currMusic==musicFiles.size()-1) currMusic=0;
            else currMusic++;
            currMusicFile = musicFiles[currMusic];
            PlayMusicStream(currMusicFile);
        }


        if (!closeWindow) closeWindow = WindowShouldClose();

    }

    CloseAudioDevice();
    CloseWindow();

}
