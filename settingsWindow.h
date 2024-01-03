#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

void SetColors();
void SettingsInitRendering();
void SettingsReshape(int w, int h);
void SettingsDisplay();
void SettingsKeyboard(unsigned char key, int x, int y);
void SettingsMouse(int button, int state, int x, int y);
void SettingsUpdate(int value);

void InitMinMaxColor();
void InitRendering();
void InitBeings();
void Reshape(int w, int h);
void Display();
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void NoDrawing();
void Update(int value);

#endif /* SETTINGSWINDOW_H */
